/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2007, Sebastian Pipping <webmaster@hartwork.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

extern "C" {
	// Including C header from C++
	#include <uriparser/Uri.h>
}

#include "CppTest/cpptest.h"
#include <memory>
using namespace Test;
using namespace std;



/* Note the closing brackets! TODO */
#define URI_TEST_IP_SIX_FAIL(x) TEST_ASSERT(URI_FALSE == uriParseIpSixA(x "]"))
#define URI_TEST_IP_SIX_PASS(x) TEST_ASSERT(URI_TRUE == uriParseIpSixA(x "]"))



class UriSuite : public Suite {

public:
	UriSuite() {
		TEST_ADD(UriSuite::testIpSixPass)
		TEST_ADD(UriSuite::testIpSixFail)
		TEST_ADD(UriSuite::testUri)
	}

private:
	void testIpSixPass() {
		// Minimum
		URI_TEST_IP_SIX_PASS("::"); // == all addresses

		// Quad length
		URI_TEST_IP_SIX_PASS("::1"); // == localhost
		URI_TEST_IP_SIX_PASS("::12");
		URI_TEST_IP_SIX_PASS("::123");
		URI_TEST_IP_SIX_PASS("::1234");

		// Ends and middle
		URI_TEST_IP_SIX_PASS("::1:2");
		URI_TEST_IP_SIX_PASS("2:1::");
		URI_TEST_IP_SIX_PASS("1::");
		URI_TEST_IP_SIX_PASS("1::2");

		// Full length
		URI_TEST_IP_SIX_PASS("2001:0db8:0100:f101:0210:a4ff:fee3:9566"); // lower hex
		URI_TEST_IP_SIX_PASS("2001:0DB8:0100:F101:0210:A4FF:FEE3:9566"); // Upper hex
		URI_TEST_IP_SIX_PASS("2001:db8:100:f101:210:a4ff:fee3:9566");
		URI_TEST_IP_SIX_PASS("2001:0db8:100:f101:0:0:0:1");

		// Legal IPv4
		URI_TEST_IP_SIX_PASS("0::1.2.3.4");
		URI_TEST_IP_SIX_PASS("::ffff:1.2.3.4");
		URI_TEST_IP_SIX_PASS("::0.0.0.0"); // Min IPv4
		URI_TEST_IP_SIX_PASS("::255.255.255.255"); // Max IPv4

		// A few more variations
		URI_TEST_IP_SIX_PASS("21ff:abcd::1");
		URI_TEST_IP_SIX_PASS("2001:db8:100:f101::1");
		URI_TEST_IP_SIX_PASS("a:b:c::0:1");
		URI_TEST_IP_SIX_PASS("a:b::0:1:2:3");
	}

	void testIpSixFail() {
		// 5 char quad
		URI_TEST_IP_SIX_FAIL("::12345");

		// Two zippers
		URI_TEST_IP_SIX_FAIL("abcd::abcd::abcd");

		// Triple-colon zipper
		URI_TEST_IP_SIX_FAIL(":::1234");
		URI_TEST_IP_SIX_FAIL("1234:::1234:1234");
		URI_TEST_IP_SIX_FAIL("1234:1234:::1234");
		URI_TEST_IP_SIX_FAIL("1234:::");

		// No quads, just IPv4
		URI_TEST_IP_SIX_FAIL("1.2.3.4");
		URI_TEST_IP_SIX_FAIL("0001.0002.0003.0004");

		// Five quads
		URI_TEST_IP_SIX_FAIL("0000:0000:0000:0000:0000:1.2.3.4");

		// Seven quads
		URI_TEST_IP_SIX_FAIL("0:0:0:0:0:0:0");
		URI_TEST_IP_SIX_FAIL("0:0:0:0:0:0:0:");
		URI_TEST_IP_SIX_FAIL("0:0:0:0:0:0:0:1.2.3.4");

		// Nine quads
		URI_TEST_IP_SIX_FAIL("0:0:0:0:0:0:0:0:0");

		// Invalid IPv4 part
		URI_TEST_IP_SIX_FAIL("::ffff:001.02.03.004"); // Leading zeros
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3.1111"); // Four char octet
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3.256"); // > 255
		URI_TEST_IP_SIX_FAIL("::ffff:311.2.3.4"); // > 155
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3:4"); // Not a dot
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3"); // Missing octet
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3."); // Missing octet
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3a.4"); // Hex in octet
		URI_TEST_IP_SIX_FAIL("::ffff:1.2.3.4:123"); // Crap input

		// Nonhex
		URI_TEST_IP_SIX_FAIL("g:0:0:0:0:0:0");
	}

	void testUri() {
		UriParser parser = { 0 };
		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "http://www.example.com/"));
		TEST_ASSERT(URI_SUCCESS == uriParseUriW(&parser, L"http://www.example.com/"));

		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "http://sourceforge.net/projects/uriparser/"));
		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "http://sourceforge.net/project/platformdownload.php?group_id=182840"));
		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "mailto:test@example.com"));
		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "../../"));
		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "file:///bin/bash"));
		TEST_ASSERT(URI_SUCCESS == uriParseUriA(&parser, "http://www.example.com/name%20with%20spaces/"));

		TEST_ASSERT(URI_ERROR == uriParseUriA(&parser, "http://www.example.com/name with spaces/"));
	}

};



int main() {
	Suite suite;
	suite.add(auto_ptr<Suite>(new UriSuite()));
    TextOutput output(TextOutput::Verbose);
    return suite.run(output, false) ? 0 : 1;
}
