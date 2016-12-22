# music-tagger

This is a small command line app that returns the tags (id3 or otherwise) from a audio file.
It is written in c++ and uses the taglib library, and returns the tags as serialized json on stdout for easy parsing (subject to change).


## Todo

* Sane error handling
* Should be able to Update tags in file
* Clean up everything
* dump and update album cover
* Add special cases for ratings

Reading
`tagreader asd.mp3`
Updating
`tagreader update asd.mp3 hej="asd" TITLE="dfhgldf"`

`tagreader update-cover asd.mp3 hej.jpeg`
`tagreader cover asd.mp3 <cover.jpeg?>`
