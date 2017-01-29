#include <string>
#include <algorithm>
#include <iostream>

#include <taglib.h>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>
#include <tfile.h>

#include <mpegfile.h>
#include <id3v2tag.h>
#include <popularimeterframe.h>
#include <attachedpictureframe.h>

#include <flacfile.h>
#include <flacpicture.h>

#include "../lib/json.hpp"
using json = nlohmann::json;

/*
  Credit to: https://forums.mp3tag.de/lofiversion/index.php?t19001.html

  Rating   WMP 12         Win 8.1 Explor   Winamp v5.666   foobar2000 1.3.2   Clementine 1.2.3   MediaMonkey 4.1.2   MusicBee 2.3.5188
  stars    writes         writes           writes          writes             writes             writes              writes
  -------  ---------------------------------------------------------------------------------------------------------------------------
  unrated  0 or no POPM   0 or no POPM     0 or no POPM    0 or no POPM       0 or no POPM       no POPM             no POPM

  0        ---            ---              ---             ---                ---                  0                   0
  0.5      ---            ---              ---             ---                ---                 13                  13
  1          1              1                1               1                  1                  1                   1
  1.5      ---            ---              ---             ---                ---                 54                  54
  2         64             64               64              64                 64                 64                  64
  2.5      ---            ---              ---             ---                ---                118                 118
  3        128            128              128             128                128                128                 128
  3.5      ---            ---              ---             ---                ---                186                 186
  4        196            196              196             196                192                196                 196
  5        255            255              255             255                255                255                 255
  4.5      ---            ---              ---             ---                ---                242                 242

  We convert all of these to a 100 point scale like flac xiph comments
*/
int popm_to_rating(int popm_rating) {
  switch(popm_rating) {
    case 0: return 0;
    case 13: return 10;
    case 1: return 20;
    case 54: return 30;
    case 64: return 40;
    case 118: return 50;
    case 128: return 60;
    case 186: return 70;
    case 196: return 80;
    case 242: return 90;
    case 255: return 100;
    default: return 0;
  }
}

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
    TagLib::MPEG::File *mp3_f =
      dynamic_cast<TagLib::MPEG::File*>(f.file());
    
    if (mp3_f->hasID3v2Tag()) {
      TagLib::ID3v2::FrameList l = mp3_f->ID3v2Tag()->frameList("POPM");

      if(!l.isEmpty()) {
        TagLib::ID3v2::PopularimeterFrame rating_frame(l.front()->render());
        data["tags"]["RATING"] = std::to_string(popm_to_rating(rating_frame.rating()));
      }
    }
  }

  std::cout << data << std::endl;
}

static const char *PictureTypeStrings[] = {
  "Other",   
  "FileIcon",  
  "OtherFileIcon",   
  "FrontCover",  
  "BackCover",   
  "LeafletPage"
  "Media"
  "LeadArtist",  
  "Artist",  
  "Conductor"
  "Band ", 
  "Composer",  
  "Lyricist",  
  "RecordingLocation"
  "DuringRecording"
  "DuringPerformance"
  "MovieScreenCapture",  
  "ColouredFish",  
  "Illustration",  
  "BandLogo",  
  "PublisherLogo"
};

void read_cover_mp3(std::string file_path) {
  TagLib::MPEG::File file(file_path.c_str());

  if(!file.hasID3v2Tag()) {
    json error = {
      {"error", "No tags in file"},
    };
    std::cout << error << std::endl;
    return;
  }

  TagLib::ID3v2::FrameList list = file.ID3v2Tag()->frameList("APIC");

  if(list.isEmpty()) {
    json error = {
      {"error", "No picture frames (APIC) in file"},
    };
    std::cout << error << std::endl;
    return;
  };

  json data = json::array();

  for (TagLib::ID3v2::FrameList::ConstIterator it = list.begin(); it != list.end(); ++it) {
    TagLib::ID3v2::AttachedPictureFrame *frame =
      dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*it);
    
    json pic = {
      {"mime_type",  frame->mimeType().toCString(true) },
      {"description",  frame->description().toCString(true) },
      {"picture_type",  PictureTypeStrings[frame->type()] },
      {"picture_b64", frame->picture().toBase64().data() }
    };

    data.push_back(pic);
  }

  std::cout << data << std::endl;
}

void read_cover_flac(std::string file_path) {
  TagLib::FLAC::File file(file_path.c_str());
  
  // Handle id3v2 tags
  if (!file.hasXiphComment()) {
    json error = {
      {"error", "No tags in file"},
    };
    std::cout << error << std::endl;
    return;
  }  

  TagLib::List<TagLib::FLAC::Picture*> list = file.pictureList();

  if(list.isEmpty()) {
    json error = {
      {"error", "No pictures in file"},
    };
    std::cout << error << std::endl;
    return;
  };

  json data = json::array();

  TagLib::List<TagLib::FLAC::Picture*>::ConstIterator it;
  for (it = list.begin(); it != list.end(); ++it) {
    TagLib::FLAC::Picture *picture =
      dynamic_cast<TagLib::FLAC::Picture*>(*it);

    json pic = {
      {"mime_type",  picture->mimeType().toCString(true) },
      {"description",  picture->description().toCString(true) },
      {"picture_type",  PictureTypeStrings[picture->type()] },
      {"picture_b64", picture->data().toBase64().data() }
    };

    data.push_back(pic);
  }

  std::cout << data << std::endl;
}


void read_cover(std::string file_path) {
  std::string extension(file_path.substr(file_path.find_last_of(".") + 1));
  std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

  if (extension == "MP3") {
    read_cover_mp3(file_path);
  } else if (extension == "FLAC")  {
    read_cover_flac(file_path);
  }
}
