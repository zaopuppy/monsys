#ifndef Z_JSON_CODEC_H__
#define Z_JSON_CODEC_H__

namespace Z_JSON {

// template
template<typename T>
inline int encode(const T &v, char *buf, uint32_t buf_len);
template<typename T>
inline int decode(      T &v, char *buf, uint32_t buf_len);
template<typename T>
inline int getlen(const T &v);


}

#endif // Z_JSON_CODEC_H__

