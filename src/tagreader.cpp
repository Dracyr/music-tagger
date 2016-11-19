#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>

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

    data["tags"] = {
      {"title", tag->title().toCString(true) },
      {"artist", tag->artist().toCString(true) },
      {"album", tag->album().toCString(true) },
      {"year", tag->year() },
      {"comment", tag->comment().toCString(true) },
      {"genre", tag->genre().toCString(true) },
      {"track", tag->track() }
    };

    // TagLib::PropertyMap tags = f.file()->properties();

    // unsigned int longest = 0;
    // for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
    //   if (i->first.size() > longest) {
    //     longest = i->first.size();
    //   }
    // }

    // for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
    //   for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j) {
        
    //     data["extended_tags"][i->first.toCString(true)] = {};
    //     // cout << i->first << endl;
    //     // cout << left << std::setw(longest) << i->first << " - " << '"' << *j << '"' << endl;
    //   }
    // }
  }

  json out = {
    {"success", true},
    {"data", data}
  };

  cout << out << endl;
  return 0;
}


// cmake ..;make;./tagreader file:///home/dracyr/Music/test.mp3  