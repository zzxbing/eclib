/*!
\file c_str.h
\brief string functions

ec library is free C++ library.

\author	 kipway@outlook.com
*/
#ifndef C_STR_H
#define C_STR_H
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#ifndef _WIN32
#include <iconv.h>
#include <ctype.h>
#endif
namespace ec
{
    inline void str_rightzero(char* s, size_t nsize)
    {
        size_t i = 0;
        if (!nsize || !s)
            return;
        s[nsize - 1] = 0;
        while (s[i++]);
        while (i < nsize)
            s[i++] = 0;
    }

    inline void str_trim(char *s)
    {
        if (!*s)
            return;
        char *sp = s, *s1 = s;
        while (*sp == ' ' || *sp == '\t' || *sp == '\n' || *sp == '\r')
            sp++;
        while (*sp)
            *s1++ = *sp++; //move
        *s1 = '\0';
        while (s1 > s)
        {
            s1--;
            if (*s1 == ' ' || *s1 == '\t' || *s1 == '\n' || *s1 == '\r')
                *s1 = 0;
            else
                break;
        }
    }

    inline char* str_ncpy(char* sd, const char* ss, size_t nsize)//like strncpy,add null to the end of sd
    {
        size_t n = 0;
        char* sret = sd;
        if (!sret)
            return sret;
        *sd = 0;
        if (!ss || !(*ss) || !nsize)
            return sd;
        while (++n < nsize && *ss)
            *sd++ = *ss++;
        *sd = 0;
        return sret;
    };

    inline size_t str_lcpy(char* sd, const char* ss, size_t nsize)// like strlcpy for linux,add null to the end of sd
    {
        size_t n = 0;
        char* sret = sd;
        if (!sret)
            return 0;
        *sd = 0;
        if (!ss || !(*ss) || !nsize)
            return 0;
        while (++n < nsize && *ss)
            *sd++ = *ss++;
        *sd = 0;
        return --n;
    };

    inline bool str_neq(const char* s1, const char* s2, size_t n)
    {
        size_t i = 0;
        while (i < n)
        {
            if (*s1 != *s2 || !(*s1) || !(*s2))
                return false;
            i++;
            s1++;
            s2++;
        }
        return true;
    }
    inline bool str_ineq(const char* s1, const char* s2, size_t n)
    {
        size_t i = 0;
        while (i < n)
        {
            if ((*s1 != *s2 && tolower(*s1) != tolower(*s2)) || !(*s1) || !(*s2))
                return false;
            i++;
            s1++;
            s2++;
        }
        return true;
    }

    ///\brief filter string
    ///
    ///sfliter support *?
    ///\param ssrc [in] src
    ///\param sfliter [in] filter str
    ///\return true success
    inline bool str_filter(const char *ssrc, const char *sfliter)
    {
        char ssub[512], cp = 0;
        char *ps = ssub, *ss = (char *)ssrc, *sf = (char *)sfliter;
        if (!ss || !sf || *sf == 0)
            return true;
        if ((*sf == '*') && (*(sf + 1) == 0))
            return true;
        while ((*sf) && (*ss))
        {
            if (*sf == '*') {
                if (ps != ssub) {
                    *ps = 0;
                    ss = strstr(ss, ssub);
                    if (!ss)
                        return false;
                    ss += (ps - ssub);
                    ps = ssub;
                }
                cp = '*';	sf++;
            }
            else if (*sf == '?') {
                if (ps != ssub) {
                    *ps = 0;
                    ss = strstr(ss, ssub);
                    if (!ss)
                        return false;
                    ss += (ps - ssub);
                    ps = ssub;
                }
                ps = ssub;
                cp = '?';		ss++;		sf++;
            }
            else
            {
                if (cp == '*')
                    *ps++ = *sf++;
                else {
                    if (*sf != *ss)
                        return false;
                    sf++;		ss++;
                }
            }
        }//while
        if (cp != '*')
        {
            if (*ss == *sf)
                return true;
            if (*sf == '*') {
                sf++;
                if (*sf == 0)
                    return true;
            }
            return false;
        }
        if (ps != ssub) {
            *ps = 0;
            ss = strstr(ss, ssub);
            if (!ss)
                return false;
            ss += (ps - ssub);
            if (!*ss)
                return true;
            return false;
        }
        return true;
    }

    /*!
    \brief get next string
    \remark cp can't be \n\r\t and space
    \param cp separate character
    \param src source string
    \param srcsize source string length
    \param pos [in/out] current position
    \param sout [out] output buffer
    \param outsize output buffer length
    */
    inline const char* str_getnextstring(const char cp, const char* src, size_t srcsize, size_t &pos, char *sout, size_t outsize)
    {
        char c;
        size_t i = 0;
        while (pos < srcsize)
        {
            c = src[pos++];
            if (c == cp)
            {
                while (i > 0)// delete tail space char
                {
                    if (sout[i - 1] != '\t' && sout[i - 1] != ' ')
                        break;
                    i--;
                }
                sout[i] = '\0';
                if (i > 0)
                    return sout;
            }
            else if (c != '\n' && c != '\r')
            {
                if (i == 0 && (c == '\t' || c == ' ')) //delete head space char
                    continue;
                sout[i++] = c;
                if (i >= outsize)
                    return 0;
            }
        }
        if (i && i < outsize && pos == srcsize)
        {
            while (i > 0) //delete tail space char
            {
                if (sout[i - 1] != '\t' && sout[i - 1] != ' ')
                    break;
                i--;
            }
            sout[i] = '\0';
            if (i > 0)
                return sout;
        }
        return 0;
    }

    inline bool char2hex(char c, unsigned char *pout)
    {
        if (c >= 'a' && c <= 'f')
            *pout = 0x0a + (c - 'a');
        else if (c >= 'A' && c <= 'F')
            *pout = 0x0a + (c - 'A');
        else if (c >= '0' && c <= '9')
            *pout = c - '0';
        else
            return false;
        return true;
    }

    /*!
    \brief utf8 fomat url translate to utf8 string,add 0 at end
    */
    inline  int url2utf8(const char* url, char sout[], int noutsize)
    {
        int n = 0;
        unsigned char h, l;
        while (*url && n < noutsize - 1)
        {
            if (*url == '%')
            {
                url++;
                if (!char2hex(*url++, &h))
                    break;
                if (!char2hex(*url++, &l))
                    break;
                sout[n++] = (char)((h << 4) | l);
            }
            else
                sout[n++] = *url++;
        }
        sout[n] = 0;
        return n;
    }

    inline void hex2string(const void* psrc, size_t sizesrc, char *sout, size_t outsize)
    {
        unsigned char uc;
        size_t i;
        const unsigned char* pu = (const unsigned char*)psrc;
        for (i = 0; i < sizesrc && 2 * i + 1 < outsize; i++)
        {
            uc = pu[i] >> 4;
            sout[i * 2] = (uc >= 0x0A) ? 'A' + (uc - 0x0A) : '0' + uc;
            uc = pu[i] & 0x0F;
            sout[i * 2 + 1] = (uc >= 0x0A) ? 'A' + (uc - 0x0A) : '0' + uc;
        }
        sout[2 * i] = 0;
    }

    inline int str_icmp(const char* a, const char* b)
    {
#ifdef _WIN32
        return stricmp(a, b);
#else
        return strcasecmp(a, b);
#endif
    }

    inline   char chr_upr(char c)
    {
        if (c >= 'a' && c <= 'z')
            return c - 'a' + 'A';
        return c;
    }

    inline char *str_upr(char *str)
    {
#ifdef _WIN32
        return _strupr(str);
#else
        char *ptr = str;
        while (*ptr) {
            if (*ptr >= 'a' && *ptr <= 'z')
                *ptr -= 'a' - 'A';
            ptr++;
        }
        return str;
#endif
    }

    inline char *str_lwr(char *str)
    {
#ifdef _WIN32
        return _strlwr(str);
#else
        char *ptr = str;
        while (*ptr) {
            if (*ptr >= 'A' && *ptr <= 'Z')
                *ptr += 'a' - 'A';
            ptr++;
        }
        return str;
#endif
    }

    /*!
    \brief GB2312 toutf-8
    \param sizeutf8 [in/out] , in sutf8 bufsize, out utf-8 code length
    */
    inline bool gb2utf8(const char* sgb, size_t sizegb, char *sutf8, size_t &sizeutf8)
    {
        *sutf8 = 0;
#ifdef _WIN32
        int i = MultiByteToWideChar(CP_ACP, 0, sgb, (int)sizegb, NULL, 0);
        wchar_t* sUnicode = new wchar_t[i + 1];
        MultiByteToWideChar(CP_ACP, 0, sgb, (int)sizegb, sUnicode, i); //to unicode

        int nout = WideCharToMultiByte(CP_UTF8, 0, sUnicode, i, sutf8, (int)sizeutf8, NULL, NULL); //to utf-8
        sizeutf8 = nout;
        if (sizeutf8 > 0)
            sutf8[sizeutf8] = 0;
        else
        {
            sizeutf8 = 0;
            sutf8[0] = 0;
        }
        return nout > 0;
#else
        iconv_t cd;
        char **pin = (char**)&sgb;
        char **pout = &sutf8;

        cd = iconv_open("UTF-8//IGNORE", "GBK");
        if (cd == (iconv_t)-1)
        {
            strncpy(sutf8, sgb, sizeutf8);
            sizeutf8 = strlen(sutf8);
            return true;
        }
        size_t inlen = sizegb;
        size_t outlen = sizeutf8;
        if (iconv(cd, pin, &inlen, pout, &outlen) == (size_t)(-1))
        {
            iconv_close(cd);
            sizeutf8 = 0;
            return false;
        }
        iconv_close(cd);
        sizeutf8 = sizeutf8 - outlen;
        if (outlen > 0)
            *sutf8 = 0;
        return true;
#endif
    }

    inline bool gb2utf8_s(const char* sgb, size_t sizegb, char *sutf8, size_t sizeout)
    {
        size_t sz = sizeout;
        return gb2utf8(sgb, sizegb, sutf8, sz);
    }

    class cAp // auto free pointer
    {
    public:
        cAp(size_t size) {
            _p = malloc(size);
            if (_p)
                _size = size;
        }
        ~cAp() {
            if (_p)
                free(_p);
        }
        template<typename T>
        inline operator T*() {
            return (T*)_p;
        }
        inline bool isempty() {
            return !_p;
        }
        inline size_t getsize() {
            return _size;
        };
        inline void* getbuf() { return _p; };
        bool resize(size_t newsize, bool bkeepdata = false)
        {
            if (bkeepdata)
            {
                void* p = malloc(newsize);
                if (!p)
                    return false;
                if (_p && _size) {
                    memcpy(p, _p, _size);
                    free(_p);
                }
                _p = p;
            }
            else
            {
                if (_p && _size)
                    free(_p);
                _p = malloc(newsize);
            }                            
            _size = _p ? newsize : 0;
            return _p != 0;
        }
    protected:
        size_t _size;
        void* _p;
    };

    class cStrSplit
    {
    public:
        cStrSplit(const char* s, const char* sfilter = "\t\x20\f\r\n", size_t sizes = 0)
        {
            if (s && *s)
            {
                _s = s;
                if (sizes)
                    _sizes = sizes;
                else
                    _sizes = strlen(s);
            }
            else
            {
                _s = 0;
                _sizes = 0;
            }
            _pos = 0;
            _sfiled[0] = 0;
            _sfilter = sfilter;
        }
    private:
        const char* _s, *_sfilter;
        size_t _sizes, _pos;
        char _sfiled[4096]; //current filed
    public:
        inline void Reset() {
            _pos = 0;
            _sfiled[0] = 0;
        }
        char* next(const char *split, char * sout = 0, size_t sizeout = 0, size_t *psize = NULL)
        {
            if (psize)
                *psize = 0;
            char c;
            size_t i = 0, lout = sizeout;
            char *so = sout;

            if (!sout || !sizeout)
            {
                so = _sfiled;
                lout = sizeof(_sfiled);
            }
            if (!_s)
                return 0;
            while (_pos < _sizes)
            {
                c = _s[_pos++];
                if (strchr(split, c))
                {
                    while (i > 0)// delete tail space char
                    {
                        if (!_sfilter || !strchr(_sfilter, so[i - 1])) //filter out  
                            break;
                        i--;
                    }
                    so[i] = '\0';
                    if (i > 0)
                    {
                        if (psize)
                            *psize = i;
                        return so;
                    }
                }
                else
                {
                    if (_sfilter && strchr(_sfilter, c)) //filter out  
                        continue; //delete head space char                    
                    so[i++] = c;
                    if (i >= lout)
                        return 0;
                }
            }

            while (i > 0) //delete tail space char
            {
                if (!_sfilter || !strchr(_sfilter, so[i - 1])) //filter out  
                    break;
                i--;
            }
            so[i] = '\0';
            if (i > 0)
            {
                if (psize)
                    *psize = i;
                return so;
            }
            return 0;
        }
    };

};//namespace ec
#endif //C_STR_H

