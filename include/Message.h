#include <cstring>
#include <stdlib.h>
#include <stdint.h>

constexpr bool TRACE_MSG{ false};

class Message
{
   public:
      static const size_t MAX_MSG_SIZE{200};

      Message() {
        printf ("Message()\n");
      };

      Message (size_t len, const void* buf)
      {
         if (TRACE_MSG) printf ("Message(size_t,void*)\n");
         mLen = len < MAX_MSG_SIZE ? len : MAX_MSG_SIZE;
         std::memcpy (mBuf, buf, mLen);
      }

      ~Message() { 
        if (TRACE_MSG) printf ("%s\n", __func__);
      }

      Message (const Message&& mOther) {
         if (TRACE_MSG) printf ("Message(const Message&&)\n");
         mLen = mOther.mLen ;
         std::memcpy (mBuf, mOther.mBuf, mLen);
      }


      Message (const Message& mOther) {
         if (TRACE_MSG) printf ("Message(const Message&)\n");

         mLen = mOther.mLen ;
         std::memcpy (mBuf, mOther.mBuf, mLen);
      }

      const Message &  operator= (const Message& mOther)
      {
         if (TRACE_MSG) printf ("operator=(const Message&)\n");

         Message m(mOther);
         return std::move(m);
      }

      void Attach( size_t len, const void* buf) 
      {
         mLen = len < MAX_MSG_SIZE ? len : MAX_MSG_SIZE;
         std::memcpy (mBuf, buf, mLen);
      }

      size_t size() const {return mLen;}
      void* GetBuffer(){ return mBuf;}

   private:

      size_t mLen{0};
      uint8_t mBuf[MAX_MSG_SIZE];
};
