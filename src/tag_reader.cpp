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
#include <xiphcomment.h>

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

json read_cover_id3v2(TagLib::ID3v2::Tag *tag) {
  TagLib::ID3v2::FrameList list = tag->frameList("APIC");
  json pictures = json::array();

  if(list.isEmpty()) {
    return pictures;
  };

  TagLib::ID3v2::FrameList::ConstIterator it;
  for (it = list.begin(); it != list.end(); ++it) {
    TagLib::ID3v2::AttachedPictureFrame *frame =
      dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*it);
    
    json pic = {
      {"mime_type",  frame->mimeType().toCString(true) },
      {"description",  frame->description().toCString(true) },
      {"picture_type",  PictureTypeStrings[frame->type()] },
      {"picture_b64", frame->picture().toBase64().data() }
    };

    pictures.push_back(pic);
  }

  return pictures;
}

json read_cover_xiph(TagLib::List<TagLib::FLAC::Picture*> list) {
  json pictures = json::array();

  if(list.isEmpty()) {
    return pictures;
  };

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

    pictures.push_back(pic);
  }

  return pictures;
}

json read_cover(TagLib::File* file) {
  std::string file_name(file->name());
  std::string extension(file_name.substr(file_name.find_last_of(".") + 1));

  if (extension == "mp3") {
    TagLib::MPEG::File *mpeg_file =
      dynamic_cast<TagLib::MPEG::File*>(file);

    if(!mpeg_file->hasID3v2Tag()) {
      return json::array();
    }
    return read_cover_id3v2(mpeg_file->ID3v2Tag());
  } else if (extension == "flac")  {
    TagLib::FLAC::File *flac_file =
      dynamic_cast<TagLib::FLAC::File*>(file);
    
    if (flac_file->hasXiphComment()) {
      return read_cover_xiph(flac_file->pictureList());
    } else if (flac_file->hasID3v2Tag()) {
      return read_cover_id3v2(flac_file->ID3v2Tag());
    } else {
      return json::array();
    }
  } else {
    return json::array();
  }
}

json audio_properties(TagLib::File *file) {
  json audioProperties;
  
  TagLib::AudioProperties *properties = file->audioProperties();

  audioProperties = {
    {"duration", properties->length()},
    {"bitrate", properties->bitrate()},
    {"sample_rate", properties->sampleRate()},
    {"channels", properties->channels()}
  };
  return audioProperties;
}

void read_tags(std::string file_path) {
  TagLib::FileRef f(file_path.c_str());
  TagLib::File *file = f.file();

  if (!file->isValid()) {
    return;
  }

  json data;
  data["audio_properties"] = audio_properties(file);

  if (file->isValid() && file->tag()) {
    TagLib::PropertyMap tags = file->properties();

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

  json cover = read_cover(file);

  if (!cover.is_null() && !cover.empty()) {
    data["cover"] = cover;
  }

  std::cout << data << std::endl;
}