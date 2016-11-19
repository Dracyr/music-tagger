#include <taglib.h>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>

#include "../lib/json.hpp"
using json = nlohmann::json;

using namespace std;

int main (int argc, char *argv[])
{
  TagLib::FileRef f(argv[1]);

  json data;

  if(!f.isNull() && f.audioProperties()) {
    TagLib::AudioProperties *properties = f.audioProperties();

    data["audio_properties"] = {
      {"duration", properties->length()},
      {"bitrate", properties->bitrate()},
      {"sample rate", properties->sampleRate()},
      {"channels", properties->channels()}
    };
  }

  if(!f.isNull() && f.tag()) {
    TagLib::Tag *tag = f.tag();
    TagLib::PropertyMap ext_tags = f.file()->properties();

    data["tags"] = {
      {"TITLE", tag->title().toCString(true) },
      {"ARTIST", tag->artist().toCString(true) },
      {"ALBUM", tag->album().toCString(true) },
      {"ALBUMARTIST", "" },      
      {"YEAR", std::to_string(tag->year()) },
      {"COMMENT", tag->comment().toCString(true) },
      {"GENRE", tag->genre().toCString(true) },
      {"TRACK", std::to_string(tag->track()) }
    };

    // Extend with property tags, since taglib will sometimes not populate tag()
    if ((tag->title() == "") && ext_tags.contains("TITLE")) {
      data["tags"]["TITLE"] = ext_tags["TITLE"][0].toCString(true);
    }
    if ((tag->artist() == "") && ext_tags.contains("ARTIST")) {
      data["tags"]["ARTIST"] = ext_tags["ARTIST"][0].toCString(true);
    }
    if ((tag->album() == "") && ext_tags.contains("ALBUM")) {
      data["tags"]["ALBUM"] = ext_tags["ALBUM"][0].toCString(true);
    }
    if (ext_tags.contains("ALBUMARTIST")) {
      data["tags"]["ALBUMARTIST"] = ext_tags["ALBUMARTIST"][0].toCString(true);
    }
    if ((tag->comment() == "") && ext_tags.contains("COMMENT")) {
      data["tags"]["COMMENT"] = ext_tags["COMMENT"][0].toCString(true);
    }
    if ((tag->genre() == "") && ext_tags.contains("GENRE")) {
      data["tags"]["GENRE"] = ext_tags["GENRE"][0].toCString(true);
    }

    if ((tag->track() == 0) && ext_tags.contains("TRACK")) {
      data["tags"]["TRACK"] = ext_tags["TRACK"][0].toCString(true);
    } else if (tag->track() == 0) {
      data["tags"]["TRACK"] = "";
    }
    if ((tag->year() == 0) && ext_tags.contains("YEAR")) {
      data["tags"]["YEAR"] = ext_tags["YEAR"][0].toCString(true);
    } else if (tag->year() == 0) {
      data["tags"]["YEAR"] = "";
    }

    // Add all of the remaining tags to the extended tag list
    unsigned int longest = 0;
    for(TagLib::PropertyMap::ConstIterator i = ext_tags.begin(); i != ext_tags.end(); ++i) {
      if (i->first.size() > longest) {
        longest = i->first.size();
      }
    }

    for(TagLib::PropertyMap::ConstIterator i = ext_tags.begin(); i != ext_tags.end(); ++i) {
      for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j) {
        if (
            i->first != "TITLE" &&
            i->first != "ARTIST" &&
            i->first != "ALBUM" &&
            i->first != "ALBUMARTIST" &&
            i->first != "YEAR" &&
            i->first != "COMMENT" &&
            i->first != "GENRE" &&
            i->first != "TRACK") {
          data["extended_ext_tags"][i->first.toCString(true)] = i->second[0].toCString(true);
        }
      }
    }
  }

  json out = {
    {"success", true},
    {"data", data}
  };

  cout << out << endl;
  return 0;
}