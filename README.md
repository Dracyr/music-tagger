# music-tagger

This is a small command line app that returns the tags from an music file.
It is written in c++ and uses the taglib library, and returns the tags as json on stdout for easy parsing.

## Usage

```
music-tagger read asd.mp3
music-tagger update <file> TITLE="New title"
```

## Support
All of the formats that taglib supports work and it will return the extended tags (ALBUMARTIST, etc). However support for reading the coverart is only available for mp3 and flac at the moment.

The important ones being:
* mp3
* flac
* m4a (No covers)
* wav (No covers)
* ogg/opus (No covers)

## Build

```
cd bin
cmake ..; make
```

## Todo

* Update album cover
* Sane error handling
* Clean up
