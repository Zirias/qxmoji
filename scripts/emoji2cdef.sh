# vim: ft=sh:sts=4:sw=4
set -f
LC_ALL=C
_groups=0

parsegroup()
{
    _group=${1#"# group: "}
    test "${1%" ${_group}"}" = "# group:" || return 1
    test -n "${_group#Component}" || return 1
    eval _group_${_groups}'_name="${_group}"'
    if [ ${_groups} -gt 0 ]; then
	echo "\
    { { 0 }, 0 }
};"
    else
    echo "\
/* Generated file, do not edit!
 *
 * run scripts/emoji2cdef.sh on contrib/emoji-test.txt to regenerate it.
 */

#include \"emoji.h\"

struct Emoji {
    char32_t codepoints[16];
    const char *name;
};

struct EmojiGroup {
    const char *name;
    const Emoji *emojis;
};
"
    fi
    echo 'static Emoji emoji_'${_groups}'_list[] = {'
    : $((_groups += 1))
    return 0
}

parseemoji()
{
    test ${_groups} -gt 0 || return 1
    _codepoints=${1%;*}
    test -n "${_codepoints}" || return 1
    _rest=${1#${_codepoints}*;}
    _name=${_rest##*qualified*#*E}
    case "${_rest%"${_name}"}" in
	*qualified*\#*E) ;;
	*) return 1
    esac
    while
	_chr=$(printf %c "${_name}")
	_name=${_name#"${_chr}"}
	test ! ${_chr} = " "
    do :; done
    test -n "${_name}" || return 1
    printf '    { { '
    for _codepoint in ${_codepoints}; do
	printf '0x%s, ' ${_codepoint}
    done
    printf '0 }, "%s" },\n' "${_name#"flag: "}"
    return 0
}

while read -r _line; do
    parsegroup "$_line" || parseemoji "$_line"
done

test ${_groups} -gt 0 || exit 1
echo "};"
echo
echo 'static EmojiGroup groups[] = {'
_group=0
while [ ${_group} -lt ${_groups} ]; do
    eval '_name="${_group_'${_group}'_name}"'
    _list=emoji_${_group}_list
    printf '    { "%s", %s },\n' "${_name}" ${_list}
    : $((_group += 1))
done
echo "\
    { 0, 0 }
};

SOLOCAL const EmojiGroup *emojigroups = groups;

SOLOCAL const char *Emoji_name(const Emoji *self)
{
    return self->name;
}

SOLOCAL const char32_t *Emoji_codepoints(const Emoji *self)
{
    return self->codepoints;
}

SOLOCAL const char *EmojiGroup_name(const EmojiGroup *self)
{
    return self->name;
}

SOLOCAL const Emoji *EmojiGroup_emojis(const EmojiGroup *self)
{
    return self->emojis;
}
"
