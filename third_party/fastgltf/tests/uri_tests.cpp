#include <catch2/catch_test_macros.hpp>

#include <fastgltf/types.hpp>

TEST_CASE("Test basic URIs", "[uri-tests]") {
    const fastgltf::URI uri1(std::string_view(""));
    REQUIRE(uri1.scheme().empty());
    REQUIRE(uri1.path().empty());

    std::string_view path = "path/somewhere.xyz";
    SECTION("Basic local path") {
        const fastgltf::URI uri2(path);
        REQUIRE(uri2.scheme().empty());
        REQUIRE(uri2.path() == path);
        REQUIRE(uri2.isLocalPath());
        REQUIRE(uri2.fspath() == path);
    }

    std::string_view abspath = "/path/somewhere.xyz";
    SECTION("File scheme path") {
        const std::string_view filePath = "file:/path/somewhere.xyz";
        const fastgltf::URI uri3(filePath);
        REQUIRE(uri3.scheme() == "file");
        REQUIRE(uri3.isLocalPath());
        REQUIRE(uri3.path() == abspath);
    }

    SECTION("File scheme localhost path") {
        const std::string_view localhostPath = "file://localhost/path/somewhere.xyz";
        const fastgltf::URI uri4(localhostPath);
        REQUIRE(uri4.scheme() == "file");
        REQUIRE(uri4.path() == abspath);
        REQUIRE(!uri4.isLocalPath());
    }
}

TEST_CASE("Test generic URIs", "[uri-tests]") {
    // These are a bunch of example URIs from https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Example_URIs
    const fastgltf::URI uri(std::string_view("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top"));
    REQUIRE(uri.scheme() == "https");
    REQUIRE(uri.userinfo() == "john.doe");
    REQUIRE(uri.host() == "www.example.com");
    REQUIRE(uri.port() == "123");
    REQUIRE(uri.path() == "/forum/questions/");
    REQUIRE(uri.query() == "tag=networking&order=newest");
    REQUIRE(uri.fragment() == "top");

    const fastgltf::URI uri1(std::string_view("ldap://[2001:db8::7]/c=GB?objectClass?one"));
    REQUIRE(uri1.scheme() == "ldap");
    REQUIRE(uri1.host() == "2001:db8::7");
    REQUIRE(uri1.path() == "/c=GB");
    REQUIRE(uri1.query() == "objectClass?one");

    const fastgltf::URI uri2(std::string_view("mailto:John.Doe@example.com"));
    REQUIRE(uri2.scheme() == "mailto");
    REQUIRE(uri2.path() == "John.Doe@example.com");

    const fastgltf::URI uri3(std::string_view("telnet://192.0.2.16:80/"));
    REQUIRE(uri3.scheme() == "telnet");
    REQUIRE(uri3.host() == "192.0.2.16");
    REQUIRE(uri3.port() == "80");
    REQUIRE(uri3.path() == "/");
}

TEST_CASE("Test percent decoding", "[uri-tests]") {
    std::string test = "%22 %25";
    fastgltf::URI::decodePercents(test);
    REQUIRE(test == "\" %");
}

TEST_CASE("Test data URI parsing", "[uri-tests]") {
    // This example base64 data is from an example on https://en.wikipedia.org/wiki/Data_URI_scheme.
    const std::string_view data = "data:image/png;base64,iVBORw0KGgoAAA"
                            "ANSUhEUgAAAAUAAAAFCAYAAACNbyblAAAAHElEQVQI12P4"
                            "//8/w38GIAXDIBKE0DHxgljNBAAO9TXL0Y4OHwAAAABJRU"
                            "5ErkJggg==";
    const fastgltf::URI uri(data);
    REQUIRE(uri.scheme() == "data");
    REQUIRE(uri.path() == data.substr(5));
}

TEST_CASE("Validate URI copying/moving", "[uri-tests]") {
    const std::string_view data = "test.bin";
    SECTION("Copy semantics") {
        fastgltf::URI uri(data);
        REQUIRE(uri.path() == data);
        fastgltf::URI uri2(uri);
        REQUIRE(uri2.raw().data() != uri.raw().data());
        REQUIRE(uri2.path() == data);
    }

    SECTION("Move semantics") {
        fastgltf::URI uri;
        {
            fastgltf::URI uri2(data);
            uri = std::move(uri2);
            REQUIRE(uri2.raw().empty());
        }
        // Test that the values were copied over and that the string views are still valid.
        REQUIRE(uri.raw() == data);
        REQUIRE(uri.path() == uri.raw());
    }
}