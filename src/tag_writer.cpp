#include <string>
#include <iostream>

#include <taglib.h>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>
#include <mpegfile.h>
#include <tfile.h>
#include <id3v2tag.h>

using namespace std;
using namespace TagLib;

int rating_to_popm(int popm_rating) {
  switch(popm_rating) {
    case 0: return 0;
    case 10: return 13;
    case 20: return 1;
    case 30: return 54;
    case 40: return 64;
    case 50: return 118;
    case 60: return 128;
    case 70: return 186;
    case 80: return 196;
    case 90: return 242;
    case 100: return 255;
    default: return 0;
  }
}

void write_mp3_tags(std::string file_path, std::map<std::string, std::string> tag_params) {
  MPEG::File f(file_path.c_str());
  PropertyMap tags = f.properties();

  for (auto &iterator : tag_params) {
    String param(iterator.first);
    String value(iterator.second);

    if (param == "RATING") {
      if (f.hasID3v2Tag()) {
        ID3v2::Tag *id3v2_tag = f.ID3v2Tag(true);
        ID3v2::FrameList fl = id3v2_tag->frameList("POPM");

        if(!fl.isEmpty()) {
          ID3v2::PopularimeterFrame *rating_frame = new ID3v2::PopularimeterFrame(fl.front()->render());
          int rating = rating_to_popm(std::stoi(value.toCString(true)));
          rating_frame->setRating(rating);

          id3v2_tag->removeFrames("POPM");
          id3v2_tag->addFrame(rating_frame);
        }
      }
    } else if (tags.contains(param)) {
      tags.replace(param, value);
    } else {
      tags.insert(param, value);
    }
  }

  f.setProperties(tags);
  f.save();
}

void write_other_tags(std::string file_path, std::map<std::string, std::string> tag_params) {
  FileRef f(file_path.c_str());
  PropertyMap tags = f.file()->properties();

  for (auto &iterator : tag_params) {
    String param(iterator.first);
    String value(iterator.second);

    if (tags.contains(param)) {
      tags.replace(param, value);
    } else {
      tags.insert(param, value);
    }
  }

  f.file()->setProperties(tags);
  f.file()->save();
}

void write_tags(std::string file_path, std::map<std::string, std::string> tag_params) {
  std::string extension(file_path.substr(file_path.find_last_of(".") + 1));

  if (extension == "mp3" || extension == "MP3") {
    write_mp3_tags(file_path, tag_params);
  } else {
    write_other_tags(file_path, tag_params);
  }
}

