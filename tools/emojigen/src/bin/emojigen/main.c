#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

typedef struct Emoji Emoji;
typedef struct EmojiGroup EmojiGroup;
typedef struct EmojiBucket EmojiBucket;

struct Emoji
{
    const EmojiGroup *group;
    char32_t *codepoints;
    char16_t *utf16;
    char *name;
};

struct EmojiGroup
{
    size_t capa;
    size_t size;
    char *name;
    Emoji **emojis;
};

struct EmojiBucket
{
    size_t capa;
    size_t size;
    size_t *emojis;
};

static size_t ngroups;
static EmojiGroup **groups;
static EmojiBucket buckets[1024];

static char line[1024];

#define skipws(c) do { while (*(c) == ' ' || *(c) == '\t') ++c; } while (0)
#define isws(c) (*(c) == ' ' || *(c) == '\t')
#define match(c, s) (!strncmp((c), (s), sizeof(s)-1) ? ((c)+=sizeof(s)-1) : 0)

static void *xmalloc(size_t sz)
{
    void *p = malloc(sz);
    if (!p) abort();
    return p;
}

static void *xrealloc(void *oldp, size_t sz)
{
    void *p = realloc(oldp, sz);
    if (!p) abort();
    return p;
}

static char *copystr(const char *s)
{
    size_t sz = strlen(s) + 1;
    char *copy = xmalloc(sz);
    memcpy(copy, s, sz);
    return copy;
}

static int hashstr(const char16_t *s)
{
    size_t h = 5381;
    while (*s) h += (h<<5) + *s++;
    return h & 0x3ffU;
}

static char16_t *toutf16(char32_t *codepoints)
{
    char16_t utf16[32] = {0};
    int len = 0;
    for (char32_t *cp = codepoints; len < 30 && *cp; ++cp)
    {
	if (*cp < 0x10000U)
	{
	    utf16[len++] = *cp;
	    continue;
	}
	char32_t cph = *cp - 0x10000U;
	utf16[len++] = 0xd800U + (cph >> 10);
	utf16[len++] = 0xdc00U + (cph & 0x3ffU);
    }
    char16_t *result = xmalloc((len+1) * sizeof *result);
    memcpy(result, utf16, (len+1) * sizeof *result);
    return result;
}

static char32_t parsecodepoint(char **c)
{
    char32_t codepoint = 0;
    skipws(*c);
    while ((**c >= '0' && **c <= '9')
	    || (**c >= 'A' && **c <= 'F'))
    {
	codepoint <<= 4;
	if (**c <= '9') codepoint += (**c - '0');
	else codepoint += (0xa + (**c - 'A'));
	++(*c);
    }
    return isws(*c) ? codepoint : 0;
}

static Emoji *Emoji_create(void)
{
    if (!ngroups) return 0;
    char32_t codepoints[16] = {0};
    int ncodepoints = 0;
    char *c = line;
    char32_t cp = 0;
    while (ncodepoints < 15 && (cp = parsecodepoint(&c)))
    {
	if (cp == 0xfe0eU || cp == 0xfe0fU)
	{
	    ncodepoints = 0;
	    break;
	}
	codepoints[ncodepoints++] = cp;
    }
    if (!ncodepoints) return 0;
    skipws(c);
    if (*c++ != ';') return 0;
    skipws(c);
    match(c, "fully-");
    match(c, "minimally-");
    match(c, "un");
    if (!match(c, "qualified")) return 0;
    skipws(c);
    if (*c++ != '#') return 0;
    while (*c && *c != 'E') ++c;
    ++c;
    while (isdigit(*c)) ++c;
    if (*c++ != '.') return 0;
    while (isdigit(*c)) ++c;
    if (!isws(c)) return 0;
    skipws(c);
    if (!*c) return 0;

    Emoji *self = xmalloc(sizeof *self);
    self->group = 0;
    self->codepoints = xmalloc((ncodepoints+1) * sizeof *self->codepoints);
    memcpy(self->codepoints, codepoints,
	    (ncodepoints+1) * sizeof *self->codepoints);
    self->utf16 = toutf16(codepoints);
    self->name = copystr(c);
    return self;
}

static void Emoji_destroy(Emoji *self)
{
    if (!self) return;
    free(self->name);
    free(self->utf16);
    free(self->codepoints);
    free(self);
}

static EmojiGroup *EmojiGroup_create(const char *name)
{
    EmojiGroup *self = xmalloc(sizeof *self);
    memset(self, 0, sizeof *self);
    self->name = copystr(name);
    return self;
}

static void EmojiGroup_destroy(EmojiGroup *self)
{
    if (!self) return;
    for (size_t i = 0; i < self->size; ++i) Emoji_destroy(self->emojis[i]);
    free(self->name);
    free(self);
}

static const char *parsegroup(void)
{
    char *c = line;
    int subgroup = 0;
    if (*c++ != '#') return 0;
    skipws(c);
    if (match(c, "sub")) subgroup = 1;
    if (!match(c, "group:")) return 0;
    skipws(c);
    if (!*c) return 0;
    if (subgroup)
    {
	if (match(c, "hand-fingers-open")) return "Hands & Body parts";
	else if (match(c, "person"))
	{
	    if (!*c || isws(c)) return "Person";
	    else if (match(c, "-role")) return "Roles & Fantasy";
	    else if (match(c, "-activity")) return "Activities";
	    else return 0;
	}
	else if (match(c, "family")) return "Family";
	else return 0;
    }
    if (match(c, "People & Body")) return 0;
    if (match(c, "Component")) return 0;
    return c;
}

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;
    FILE *emojitxt = 0;
    EmojiGroup *group = 0;

    if (argc < 2)
    {
	fputs("Usage: emojigen emoji-test.txt\n", stderr);
	goto done;
    }

    if (!(emojitxt = fopen(argv[1], "r")))
    {
	fprintf(stderr, "Error opening `%s' for reading\n", argv[1]);
	goto done;
    }

    while (fgets(line, sizeof line, emojitxt))
    {
	line[strcspn(line, "\n")] = 0;
	const char *groupname = parsegroup();
	if (groupname)
	{
	    group = EmojiGroup_create(groupname);
	    groups = xrealloc(groups, (ngroups+1) * sizeof *groups);
	    groups[ngroups++] = group;
	    continue;
	}
	Emoji *emoji = 0;
	if ((emoji = Emoji_create()))
	{
	    emoji->group = group;
	    if (group->size == group->capa)
	    {
		group->capa += 64;
		group->emojis = xrealloc(group->emojis,
			group->capa * sizeof *group->emojis);
	    }
	    group->emojis[group->size++] = emoji;
	}
    }

    int parsed = ngroups;
    for (size_t i = 0; i < ngroups; ++i)
    {
	if (!groups[i]->size)
	{
	    parsed = 0;
	    break;
	}
    }
    if (!parsed)
    {
	fprintf(stderr, "No emoji data found in `%s'\n", argv[1]);
	goto done;
    }

    puts("/* GENERATED FILE -- do not edit. */\n\n"
	    "extern const EmojiGroup emojiint_groups[];\n\n"
	    "static const Emoji emojiint_emojis[] = {");

    size_t offset = 0;
    for (size_t i = 0; i < ngroups; ++i)
    {
	group = groups[i];
	for (size_t j = 0; j < group->size; ++j)
	{
	    Emoji *emoji = group->emojis[j];
	    if (i||j) puts(",");
	    printf("    { emojiint_groups+%zu, U\"", i);
	    for (char32_t *c = emoji->codepoints; *c; ++c)
	    {
		printf("\\x%x", *c);
	    }
	    fputs("\", QStringLiteral(u\"", stdout);
	    for (char16_t *c = emoji->utf16; *c; ++c)
	    {
		printf("\\x%x", *c);
	    }
	    printf("\"), \"%s\" }", emoji->name);

	    int hash = hashstr(emoji->utf16);
	    if (buckets[hash].size == buckets[hash].capa)
	    {
		buckets[hash].emojis = xrealloc(buckets[hash].emojis,
			(buckets[hash].capa += 8) *
			sizeof buckets[hash].emojis);
	    }
	    buckets[hash].emojis[buckets[hash].size++] = offset++;
	}
    }
    offset = 0;
    puts("\n};\n\n"
	    "SOLOCAL const EmojiGroup emojiint_groups[] = {");
    for (size_t i = 0; i < ngroups; ++i)
    {
	if (i) puts(",");
	group = groups[i];
	printf("    { %zu, \"%s\", emojiint_emojis+%zu }", group->size,
		group->name, offset);
	offset += group->size;
    }
    puts("\n};\n");
    for (size_t i = 0; i < sizeof buckets / sizeof *buckets; ++i)
    {
	if (!buckets[i].size) continue;
	printf("static const Emoji *const emojiint_bucket_%zu[] = { ", i);
	for (size_t j = 0; j < buckets[i].size; ++j)
	{
	    if (j) fputs(", ", stdout);
	    printf("emojiint_emojis+%zu", buckets[i].emojis[j]);
	}
	puts("};");
    }
    puts("\nstatic const EmojiBucket emojiint_hashtable[] = {");
    for (size_t i = 0; i < sizeof buckets / sizeof *buckets; ++i)
    {
	if (i) puts(",");
	if (buckets[i].size)
	{
	    printf("    { %zu, emojiint_bucket_%zu }", buckets[i].size, i);
	}
	else fputs("    { 0, 0}", stdout);
    }
    puts("\n};");

    rc = EXIT_SUCCESS;
done:
    if (groups)
    {
	for (size_t i = 0; i < sizeof buckets / sizeof *buckets; ++i)
	{
	    free(buckets[i].emojis);
	}
	for (size_t i = 0; i < ngroups; ++i) EmojiGroup_destroy(groups[i]);
	free(groups);
    }
    if (emojitxt) fclose(emojitxt);
    return rc;
}
