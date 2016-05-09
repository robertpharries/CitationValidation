#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <jsoncpp/json/json.h>

using namespace std;

int main( int argc, char **argv ) {
    std::string base = "http://api.elsevier.com/content/abstract/doi/10.1049/iet-sen.2009.0084?apiKey=eb697e3061f267d5945a3bc3c959874a&view=REF&httpAccept=application/json";
    std::string function = "access/list";
    std::string username = "testuser";
    std::string password = "testpass";

    std::ostringstream os;
    curlpp::Easy c;
    Json::Value root;
    Json::Reader reader;

    c.setOpt( new curlpp::options::Url( base ) );
    c.setOpt( new curlpp::options::FollowLocation( "true" ) );
    os << c;
    cout << os.str() << endl;
    reader.parse( os.str(), root, false );
    const Json::Value data = root[ "data" ];
    // for ( unsigned int index = 0; index < data.size(); ++index ) {
    //     const Json::Value v = data[ index ];
    //     cout << "Remote access account id=" << v[ "id" ].asString() << ", name=" << v[ "name" ].asString() << endl;
    // }

    return 0;
}