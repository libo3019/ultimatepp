#ifndef _lz4_lz4_h_
#define _lz4_lz4_h_

#include <Core/Core.h>

#include <Core/lib/lz4.h>

namespace Upp {

enum {
	LZ4F_MAGIC       = 0x184D2204,

	LZ4F_VERSIONMASK = 0b11000000,
	LZ4F_VERSION     = 0b01000000,
	LZ4F_BLOCKINDEPENDENCE = (1 << 5),
	LZ4F_BLOCKCHECKSUM     = (1 << 4),
	LZ4F_CONTENTSIZE       = (1 << 3),
	LZ4F_CONTENTCHECKSUM   = (1 << 2),
	
	LZ4F_MAXSIZEMASK      = 0x70,
    LZ4F_MAXSIZE_64KB     = 0x40,
    LZ4F_MAXSIZE_256KB    = 0x50,
    LZ4F_MAXSIZE_1024KB   = 0x60,
    LZ4F_MAXSIZE_4096KB   = 0x70,
};

class Lz4 {
	StringBuffer buffer; // to be able to pass as String
	int8         compress;
	bool         error;
	
	enum { BLOCK_BYTES = 1024 * 1024 };
	
	xxHashStream xxh;
	int          maxblock;
	int          pos;
	int          blockchksumsz;
	bool         header;
	byte         lz4hdr;
	String       header_data;

    String       out;
	
	bool         parallel;
	CoWork       co;
	Mutex        lock;
	ConditionVariable cond;
	int          outblock;
	int          inblock;
    
    void          TryHeader();

	void          Init();
	void          FinishBlock(char *outbuf, int clen, const char *origdata, int origsize);
	void          FlushOut();

	void          PutOut(const void *ptr, int size);

public:	
	Callback2<const void *, int> WhenOut;

	void Put(const void *ptr, int size);
	void Put(const String& s)              { Put(~s, s.GetCount()); }
	void End();
	void Clear();
	
	const String& Get() const              { return out; }
	operator const String&() const         { return out; }
	const String& operator~() const        { return out; }
	void   ClearOut()                      { out.Clear(); }

	void Compress();
	void Decompress();
	
	void Parallel(bool b = true)           { parallel = b; }

	bool   IsError() const                 { return error; }

	Lz4();
	~Lz4();
};

class LZ4CompressStream : public Stream  {
public:
	virtual   void  Close();
	virtual   bool  IsOpen() const;

protected:
	virtual   void  _Put(int w);
	virtual   void  _Put(const void *data, dword size);
	
	Stream      *out;

	StringBuffer buffer; // to be able to pass it as String
	
	enum { BLOCK_BYTES = 1024 * 1024 };
	
	xxHashStream xxh;
	int          maxblock;
	int          blockchksumsz;
	bool         header;
	byte         lz4hdr;
	String       header_data;

    String       sout;
	
	bool              parallel;
	CoWork            co;
	Mutex             lock;
	ConditionVariable cond;
	int               outblock;
	int               inblock;
    
	void          Init();
	void          SetupBuffer();
	void          FinishBlock(char *outbuf, int clen, const char *origdata, int origsize);
	void          FlushOut();

	void          PutOut(const void *ptr, int size);

public:
	Event<int64>                 WhenPos;

	void Clear();
	
	const String& Get() const              { return sout; }
	operator const String&() const         { return sout; }
	const String& operator~() const        { return sout; }
	void   ClearOut()                      { sout.Clear(); }

	void Parallel(bool b = true)           { parallel = b; }
	void Out(Stream& o)                    { out = &o; }

	LZ4CompressStream();
	~LZ4CompressStream();
};

int64  LZ4Compress(Stream& out, Stream& in, Gate2<int64, int64> progress = false);
int64  LZ4Decompress(Stream& out, Stream& in, Gate2<int64, int64> progress = false);
String LZ4Compress(const void *data, int64 len, Gate2<int64, int64> progress = false);
String LZ4Compress(const String& s, Gate2<int64, int64> progress = false);
String LZ4Decompress(const void *data, int64 len, Gate2<int64, int64> progress = false);
String LZ4Decompress(const String& s, Gate2<int64, int64> progress = false);

int64  CoLZ4Compress(Stream& out, Stream& in, Gate2<int64, int64> progress = false);
int64  CoLZ4Decompress(Stream& out, Stream& in, Gate2<int64, int64> progress = false);
String CoLZ4Compress(const void *data, int64 len, Gate2<int64, int64> progress = false);
String CoLZ4Compress(const String& s, Gate2<int64, int64> progress = false);
String CoLZ4Decompress(const void *data, int64 len, Gate2<int64, int64> progress = false);
String CoLZ4Decompress(const String& s, Gate2<int64, int64> progress = false);

bool IsLZ4(Stream& s);

};

#endif
