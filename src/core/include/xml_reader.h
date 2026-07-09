#pragma once

#ifdef _WIN32
#ifdef XML_READER_EXPORTS
#define XML_READER_API __declspec(dllexport)
#else
#define XML_READER_API __declspec(dllimport)
#endif
#else
#define XML_READER_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define XML_READER_MAX_RECTS 128

typedef struct XmlReaderWindowConfig {
  char title[256];
  int width;
  int height;
  int fullscreen;
} XmlReaderWindowConfig;

typedef struct XmlReaderGraphicsConfig {
  int target_fps;
  int vsync;
} XmlReaderGraphicsConfig;

typedef struct XmlReaderRect {
  int id;
  float x;
  float y;
  float width;
  float height;
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
  int filled; /* 0 or 1 */
  int normalized;
} XmlReaderRect;

typedef struct XmlReaderConfig {
  XmlReaderWindowConfig window;
  XmlReaderGraphicsConfig graphics;
  XmlReaderRect rects[XML_READER_MAX_RECTS];
  int rect_count;
} XmlReaderConfig;

XML_READER_API int xml_reader_load_from_file(const char* path, XmlReaderConfig* out_config);
XML_READER_API int xml_reader_reload_from_file(const char* path, XmlReaderConfig* out_config);

#ifdef __cplusplus
}
#endif