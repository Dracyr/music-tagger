#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>

#include <taglib.h>
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>

#include "../lib/json.hpp"

#include <gst/gst.h>

using json = nlohmann::json;

using namespace std;

#include <gst/gst.h>
#include <glib.h>

static GMainLoop *loop;

static gboolean my_bus_callback(GstBus *bus, GstMessage *message, gpointer data)
{

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      g_free (debug);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_STATE_CHANGED:
      GstState old_state, new_state;
      
      gst_message_parse_state_changed(message, &old_state, &new_state, NULL);
      // g_print("Element %s changed state from %s to %s.\n",
      //     GST_OBJECT_NAME(message->src),
      //     gst_element_state_get_name(old_state),
      //     gst_element_state_get_name(new_state));

      break;
    default:
      g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
      /* unhandled message */
      break;
  }

  /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
  return TRUE;
}


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

  GstElement *play;
  GstBus *bus;

  /* init GStreamer */
  gst_init (&argc, &argv);
  loop = g_main_loop_new(NULL, FALSE);

  /* make sure we have a URI */
  if (argc != 2) {
    g_print("Usage: %s <URI>\n", argv[0]);
    return -1;
  }

  std::string result = std::string("file://") + std::string(argv[1]);
  /* set up */
  play = gst_element_factory_make("playbin", "play");
  g_object_set(G_OBJECT (play), "uri", result.c_str(), NULL);
  
  bus = gst_pipeline_get_bus(GST_PIPELINE (play));
  gst_bus_add_watch(bus, my_bus_callback, loop);
  gst_object_unref(bus);

  gst_element_set_state(play, GST_STATE_PLAYING);

  /* now run */
  g_main_loop_run(loop);

  /* also clean up */
  gst_element_set_state(play, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT (play));

  return 0;
}


// cmake ..;make;./tagreader file:///home/dracyr/Music/test.mp3  