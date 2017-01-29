# music-tagger

This is a small command line app that returns the tags (id3 or otherwise) from an audio file.
It is written in c++ and uses the taglib library, and returns the tags as serialized json on stdout for easy parsing (subject to change).

## Usage
Reading

```
music-tagger read asd.mp3
music-tagger read-cover <file>
```

Updating

```
music-tagger update <file> TITLE="New title"
```

## Todo

* Handle more than mp3/flac
* Update album cover
* Sane error handling
* Clean up
