#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional/optional.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <vector>

#include "getApi.h"

std::vector<ref*> getFromApi(string doi)
{
	std::vector<ref*> reflist;
	ref* curref = NULL;

    try
    {
        std::stringstream ss;
        // send your JSON above to the parser below, but populate ss first

	    std::string refurl = "http://api.elsevier.com/content/abstract/doi/" + doi + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=REF&httpAccept=application/json";

	    curlpp::Easy c;

	    c.setOpt( new curlpp::options::Url( refurl ) );
	    c.setOpt( new curlpp::options::FollowLocation( "true" ) );
	    ss << c;

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

        int i = 0;

        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("abstracts-retrieval-response.references.reference")){
            curref = new ref;

            boost::property_tree::ptree ptr = v.second;

           	curref->sourceTitle = ptr.get<string>("sourcetitle");

            BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, ptr){
            	std::string metaurl;

            	// Create current refenence struct to fill

    			if(!strcmp(v2.first.data(), "author-list") && strcmp(v2.second.data().c_str(), "null")) {
    				boost::property_tree::ptree ptr2 = v2.second;
   					BOOST_FOREACH(boost::property_tree::ptree::value_type &v3, ptr2.get_child("author")){
   						boost::property_tree::ptree ptr3 = v3.second;

   						curref->authors.push_back(ptr3.get<string>("ce:indexed-name"));

   					}
    			}

    			if (!strcmp(v2.first.data(), "scopus-id")) {
    				metaurl = "http://api.elsevier.com/content/abstract/scopus_id/" + v2.second.data() + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=META&httpAccept=application/json";

    				// Clear stringstream
		        	ss.flush();

				    c.setOpt( new curlpp::options::Url( metaurl ) );
	    			c.setOpt( new curlpp::options::FollowLocation( "true" ) );
	    			ss << c;

		        	boost::property_tree::ptree rpt;
		        	boost::property_tree::read_json(ss, rpt);

        		    curref->title = "[No title available]";
	        		curref->volume = "";
	        		curref->issue = "";
	        		curref->pageStart = "";
	        		curref->pageEnd = "";
	        		curref->doi = "";

		        	BOOST_FOREACH(boost::property_tree::ptree::value_type &vtec, rpt) {
		        		if (strcmp(vtec.first.data(), "service-error")) {
		        			curref->title = rpt.get<string>("abstracts-retrieval-response.coredata.dc:title");
				        	std::vector<string> strs;
				        	string barp = rpt.get<string>("abstracts-retrieval-response.coredata.prism:coverDate");
			        		boost::split(strs, barp, boost::is_any_of("-"));
			        		curref->year = strs[0];

			        		boost::optional< boost::property_tree::ptree& > child;

							child = rpt.get_child_optional( "abstracts-retrieval-response.coredata.prism:volume" );
							if( child ) {
			            		curref->volume = rpt.get<string>("abstracts-retrieval-response.coredata.prism:volume");
							}
							child = rpt.get_child_optional( "abstracts-retrieval-response.coredata.prism:issueIdentifier" );
							if( child ) {
			            		curref->issue = rpt.get<string>("abstracts-retrieval-response.coredata.prism:issueIdentifier");
							}
							child = rpt.get_child_optional( "abstracts-retrieval-response.coredata.prism:startingPage" );
							if( child ) {
			            		curref->pageStart = rpt.get<string>("abstracts-retrieval-response.coredata.prism:startingPage");
							}
							child = rpt.get_child_optional( "abstracts-retrieval-response.coredata.prism:endingPage" );
							if( child ) {
			            		curref->pageEnd = rpt.get<string>("abstracts-retrieval-response.coredata.prism:endingPage");
							}
							child = rpt.get_child_optional( "abstracts-retrieval-response.coredata.prism:doi" );
							if( child ) {
			            		curref->doi = rpt.get<string>("abstracts-retrieval-response.coredata.prism:doi");
							}
			            	curref->status = "unsure";
			        	}
			        		for(int i = 0; i < curref->authors.size(); i++)
			        			cout << "curref->authors " << i << " " << curref->authors.at(i) << endl;
			        		cout << "curref->title " << curref->title << endl;
			        		cout << "curref->sourceTitle " << curref->sourceTitle << endl;
			            	cout << "curref->volume " << curref->volume << endl;
			            	cout << "curref->issue " << curref->issue << endl;
			            	cout << "curref->pageStart " << curref->pageStart << endl;
			            	cout << "curref->pageEnd " << curref->pageEnd << endl;
			            	cout << "curref->doi " << curref->doi << endl;
			            	cout << "curref->status " << curref->status << endl;
			        }
    			}
        	}

        	reflist.push_back(curref);
		}
        // return EXIT_SUCCESS;
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return reflist;
}

