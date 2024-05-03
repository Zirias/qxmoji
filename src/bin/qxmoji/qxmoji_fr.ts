<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR">
<context>
    <name>EmojiGroup</name>
    <message>
        <location filename="emojidata.cpp" line="3782"/>
        <source>Smileys &amp; Emotion</source>
        <translation>Visages &amp; Émotions</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3783"/>
        <source>Hands &amp; Body parts</source>
        <translation>Parties du corps</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3784"/>
        <source>Person</source>
        <translation>Personnes</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3785"/>
        <source>Roles &amp; Fantasy</source>
        <translation>Rôles &amp; Imagination</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3786"/>
        <source>Activities</source>
        <translation>Activitées</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3787"/>
        <source>Family</source>
        <translation>Famille</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3788"/>
        <source>Animals &amp; Nature</source>
        <translation>Animaux &amp; Nature</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3789"/>
        <source>Food &amp; Drink</source>
        <translation>Manger &amp; Boissons</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3790"/>
        <source>Travel &amp; Places</source>
        <translation>Voyager &amp; Endroits</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3791"/>
        <source>Objects</source>
        <translation>Objets</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3792"/>
        <source>Symbols</source>
        <translation>Symboles</translation>
    </message>
    <message>
        <location filename="emojidata.cpp" line="3793"/>
        <source>Flags</source>
        <translation>Drapeaus</translation>
    </message>
</context>
<context>
    <name>QDialog</name>
    <message>
        <location filename="aboutdlg.cpp" line="13"/>
        <source>About qXmoji</source>
        <translation>À propos de qXmoji</translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="30"/>
        <source>X11 emoji keyboard with a Qt GUI</source>
        <translation>X11 clavier emoji avec interface Qt</translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="31"/>
        <source>License</source>
        <translation>Licence</translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="32"/>
        <source>Author</source>
        <translation>Auteur</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="14"/>
        <source>qXmoji settings</source>
        <translation>Configurer qXmoji</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="23"/>
        <source>Instance mode:</source>
        <translation>Mode d&apos;occurrence:</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="24"/>
        <source>In &quot;single&quot; mode, only one instance of qXmoji is allowed to
run per user on the local machine, and starting another one
will just bring the running instance to the front.
In &quot;multi&quot; mode, no checks for already running instances
are done.</source>
        <translation>En mode &quot;seul&quot;, une seule occurence de qXmoji peut être active
par utilisateur et ordinateur. Démarrer une autre seulement expose
celle qui est déjà active.
En mode &quot;plusieurs&quot;, il n&apos;y à pas de restrictions.</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="32"/>
        <source>Single</source>
        <translation>Seul</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="33"/>
        <source>Multi</source>
        <translation>Plusieurs</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="36"/>
        <source>Tray mode:</source>
        <translation>Mode barre système:</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="37"/>
        <source>Disabled: Do not use a tray icon.
Enabled: Use a tray icon, qXmoji keeps running on window close.
Minimize: Use a tray icon with &quot;minimize to tray&quot;, closing
the window still exits.</source>
        <translation>Désactivé: N&apos;utilise pas une icône au barre système.
Activé: Utilise une icône et continue exécuter qXmoji
quand la fenêtre est fermée.
Minimiser: Utilise une icône en mode &quot;minimiser au barre
système&quot;, fermer la fenêtre quitte qXmoji.</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="52"/>
        <source>Scale size:</source>
        <translation>Niveau de zoom:</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="53"/>
        <source>Scale up the size of displayed Emojis.
Tiny means no scaling (same size as default window font)</source>
        <translation>Agrandis les emojis pour affichage.
&quot;Minuscule&quot; n&apos;agrandis pas (même dimension que le texte)</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="67"/>
        <source>Keymap reset wait (ms):</source>
        <translation>Remettre disposition clavier après (ms):</translation>
    </message>
    <message>
        <location filename="settingsdlg.cpp" line="68"/>
        <source>qXmoji sends emoji codepoints as key press events.
Therefore, the keyboard mapping must be temporarily changed.
This is the time in ms to wait before restoring the mapping
after sending the keyboard events.
Raising this might help if emojis don&apos;t &quot;arrive&quot; at all or you
see basic (ASCII) characters instead.
It won&apos;t help when a single emoji is shown as two or more
symbols, this is handled by the receiving client.</source>
        <translation>qXmoji envois des emojis en événements de clavier. Pur ça,
il faut changer la disposition du clavier. Ceci es le délai avant
remettre la disposition originale, après les événements sont
envois.
Agrandir ce temps pourrait améliorer le fonctionnement si
quelques emojis n&apos;arrivent pas du tout ou si des signes
&quot;simples&quot; (ASCII) arrivent.
Mais si le résultat est plusieurs emblèmes, c&apos;est un problème
qui se trouve au destinataire.</translation>
    </message>
</context>
<context>
    <name>QXmoji</name>
    <message>
        <location filename="qxmoji.cpp" line="78"/>
        <source>Show &amp;Window</source>
        <translation>Montrer la &amp;fenêtre</translation>
    </message>
    <message>
        <location filename="qxmoji.cpp" line="79"/>
        <source>&amp;About</source>
        <translation>&amp;A propos de</translation>
    </message>
    <message>
        <location filename="qxmoji.cpp" line="80"/>
        <source>&amp;Settings</source>
        <translation>&amp;Configurer</translation>
    </message>
    <message>
        <location filename="qxmoji.cpp" line="81"/>
        <source>E&amp;xit</source>
        <translation>&amp;Quitter</translation>
    </message>
    <message>
        <location filename="qxmoji.cpp" line="118"/>
        <source>Emoji keyboard</source>
        <translation>Clavier emoji</translation>
    </message>
</context>
<context>
    <name>QXmojiWin</name>
    <message>
        <location filename="qxmojiwin.cpp" line="158"/>
        <source>Search</source>
        <translation>Recherche</translation>
    </message>
    <message>
        <location filename="qxmojiwin.cpp" line="176"/>
        <source>History</source>
        <translation>Récent</translation>
    </message>
</context>
<context>
    <name>Scale</name>
    <message>
        <location filename="emojifont.cpp" line="15"/>
        <source>Tiny</source>
        <translation>Minuscule</translation>
    </message>
    <message>
        <location filename="emojifont.cpp" line="16"/>
        <source>Small</source>
        <translation>Petit</translation>
    </message>
    <message>
        <location filename="emojifont.cpp" line="17"/>
        <source>Medium</source>
        <translation>Moyen</translation>
    </message>
    <message>
        <location filename="emojifont.cpp" line="18"/>
        <source>Large</source>
        <translation>Grand</translation>
    </message>
    <message>
        <location filename="emojifont.cpp" line="19"/>
        <source>Huge</source>
        <translation>Immense</translation>
    </message>
</context>
<context>
    <name>SearchField</name>
    <message>
        <location filename="searchfield.cpp" line="7"/>
        <source>Click to type and search ...</source>
        <translation>Cliquer pour taper et chercher ...</translation>
    </message>
</context>
<context>
    <name>TrayMode</name>
    <message>
        <location filename="qxmoji.cpp" line="28"/>
        <source>Disabled</source>
        <translation>Désactivé</translation>
    </message>
    <message>
        <location filename="qxmoji.cpp" line="29"/>
        <source>Enabled</source>
        <translation>Activé</translation>
    </message>
    <message>
        <location filename="qxmoji.cpp" line="30"/>
        <source>Minimize</source>
        <translation>Minimiser</translation>
    </message>
</context>
</TS>
