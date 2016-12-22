#include <string>
#include <iostream>

#include <taglib.h>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>
#include <mpegfile.h>
#include <tfile.h>
#include <id3v2tag.h>
#include <popularimeterframe.h>

#include "../lib/json.hpp"
using json = nlohmann::json;

void read_tags(std::string file_path) {
  TagLib::FileRef f(file_path.c_str());

  json data;

  if (!f.isNull() && f.audioProperties()) {
    TagLib::AudioProperties *properties = f.audioProperties();

    data["audio_properties"] = {
      {"duration", properties->length()},
      {"bitrate", properties->bitrate()},
      {"sample_rate", properties->sampleRate()},
      {"channels", properties->channels()}
    };
  }

  if (!f.isNull() && f.tag()) {
    TagLib::PropertyMap tags = f.file()->properties();

    for (TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
      for (TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j) {
        data["tags"][i->first.toCString(true)] = i->second[0].toCString(true);
      }
    }
  }

  // Add special handling of rating field for id3v2 tags
  std::string extension(file_path.substr(file_path.find_last_of(".") + 1));
  if (extension == "mp3" || extension == "MP3") {
    TagLib::MPEG::File mp3_f(file_path.c_str());
    if (mp3_f.hasID3v2Tag()) {
      TagLib::ID3v2::FrameList l = mp3_f.ID3v2Tag()->frameList("POPM");

      if(!l.isEmpty()) {
        TagLib::ID3v2::PopularimeterFrame rating_frame(l.front()->render());
        data["tags"]["RATING"] = rating_frame.rating();
      }
    }
  }

  std::cout << data << std::endl;
}
