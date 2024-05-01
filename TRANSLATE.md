# How to add a new translation

This will guide you through the process of adding a translation for the
hypothetical language "Xylophone" with language-code `xy`.

### Obtain translated emoji names

First navigate to the latest release (at the time of writing `release-45`)
of the [Unicode CLDR](https://github.com/unicode-org/cldr). From the tree
(there's a link on the release page), fetch the `xy.xml` files from
`common/annotations` and `common/annotationsDerived` and place them in the
respective directories in `src/bin/qxmoji/contrib/cldr`.

### Add the new language

In `src/bin/qxmoji/qxmoji.mk`, add `xy` to the `qxmoji_QT_LANGUAGES` variable.
Please keep the language codes sorted alphabetically.

Then run

    make update-translations

This will create two new files in `src/bin/qxmoji`, `emojidata_xy.ts` and
`qxmoji_xy.ts`. The first of them is already completely translated from the
CLDR files. It will always be generated in the build, therefore please add it
to the `.gitignore` file.

### Translate the texts

Run the Qt Linguist tool on `src/bin/qxmoji/qxmoji_xy.ts`. Translate all the
texts to "Xylophone" until every single text has a green checkmark on it.
Then save the file.

### Finish

Now, building and installing qXmoji should give you a version full localized
for "Xylophone".

`git add` the new CLDR files, `src/bin/qxmoji/qxmoji_xy.ts` and
`src/bin/qxmoji/qxmoji.mk`, create a commit, and please, send a pull request,
thank you very much! 🤩

