#include <cxxtest/TestSuite.h>

#include "lib/lib.h"
#include "lib/self_test.h"
#include "lib/res/file/compression.h"

class TestCompression : public CxxTest::TestSuite 
{
public:
	void test1()
	{
		// generate random input data
		const size_t data_size = 10000;
		u8 data[data_size];
		for(size_t i = 0; i < data_size; i++)
			data[i] = rand() & 0xFF;

		void* cdata; size_t csize;
		u8 ucdata[data_size];

		// compress
		uintptr_t c = comp_alloc(CT_COMPRESSION, CM_DEFLATE);
		{
		TS_ASSERT(c != 0);
		TS_ASSERT_OK(comp_alloc_output(c, data_size));
		const ssize_t cdata_produced = comp_feed(c, data, data_size);
		TS_ASSERT(cdata_produced > 0);
		TS_ASSERT_OK(comp_finish(c, &cdata, &csize));
		TS_ASSERT(cdata_produced <= csize);	// can't have produced more than total
		}

		// decompress
		uintptr_t d = comp_alloc(CT_DECOMPRESSION, CM_DEFLATE);
		{
		TS_ASSERT(d != 0);
		TS_ASSERT_OK(comp_set_output(ucdata, data_size));
		const ssize_t ucdata_produced = comp_feed(c, cdata, cdata_size);
		TS_ASSERT(ucdata_produced > 0);
		void* ucdata_final; size_t ucsize_final;
		TS_ASSERT_OK(comp_finish(c, &ucdata_final, &ucsize_final));
		TS_ASSERT(ucdata_produced <= ucsize_final);	// can't have produced more than total
		TS_ASSERT_EQUALS(ucdata_final, ucdata);	// output buffer address is same
		TS_ASSERT_EQUALS(ucsize_final, data_size);	// correct amount of output
		}

		// verify data survived intact
		TS_ASSERT_SAME_DATA(data, ucdata, data_size);
	}
};
