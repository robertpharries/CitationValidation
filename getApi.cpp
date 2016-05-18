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

std::vector<ref*> getFromApi(string doi, doiIO d)
{
	std::vector<ref*> reflist;
	std::vector<corRef*> corRefList;
	ref* curref = NULL;
	corRef* corCurRef = NULL;

    try
    {
        std::stringstream ss;
        // send your JSON above to the parser below, but populate ss first

	    std::string refurl = "http://api.elsevier.com/content/abstract/doi/" + doi + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=REF&httpAccept=application/json";

	    std::cout << refurl << std::endl;

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
			        		// for(int i = 0; i < curref->authors.size(); i++)
			        		// 	cout << "curref->authors " << i << " " << curref->authors.at(i) << endl;
			        		// cout << "curref->title " << curref->title << endl;
			        		// cout << "curref->sourceTitle " << curref->sourceTitle << endl;
			          //   	cout << "curref->volume " << curref->volume << endl;
			          //   	cout << "curref->issue " << curref->issue << endl;
			          //   	cout << "curref->pageStart " << curref->pageStart << endl;
			          //   	cout << "curref->pageEnd " << curref->pageEnd << endl;
			          //   	cout << "curref->doi " << curref->doi << endl;
			          //   	cout << "curref->status " << curref->status << endl;
			        }
    			}
        	}

        	// Perform a search here on the reference to make sure the information we have from the API is correct
        	corCurRef = new corRef;
        	corCurRef->title = "";
        	corCurRef->year = "";
        	corCurRef->sourceTitle = "";
        	corCurRef->volume = "";
        	corCurRef->issue = "";
        	corCurRef->pageStart = "";
        	corCurRef->pageEnd = "";
        	corCurRef->doi = "";

        	// Grab the scopus ID from the current reference
        	string sid = ptr.get<string>("scopus-id");

        	// Reset the stringstream and query the API again
        	ss.flush();
        	string metaurl = "http://api.elsevier.com/content/abstract/scopus_id/" + sid + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=META&httpAccept=application/json";
        	c.setOpt(new curlpp::options::Url(metaurl));
        	c.setOpt(new curlpp::options::FollowLocation("true"));
        	ss << c;

	        boost::property_tree::ptree cpt;
        	boost::property_tree::read_json(ss, cpt);

        	// Check all the fields
        	boost::optional< boost::property_tree::ptree& > corTitle = cpt.get_child_optional("abstracts-retrieval-response.coredata.dc:title");
        	boost::optional< boost::property_tree::ptree& > corYear = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:coverDate");
        	boost::optional< boost::property_tree::ptree& > corSourceTitle = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:publicationName");
        	boost::optional< boost::property_tree::ptree& > corVolume = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:volume");
        	boost::optional< boost::property_tree::ptree& > corIssue = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:issueIdentifier");
        	boost::optional< boost::property_tree::ptree& > corPageStart = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:startingPage");
        	boost::optional< boost::property_tree::ptree& > corPageEnd = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:endingPage");
        	boost::optional< boost::property_tree::ptree& > corDoi = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:doi");

        	if (corTitle) {
        		string newTitle = cpt.get<string>("abstracts-retrieval-response.coredata.dc:title");
        		if (curref->title != newTitle) {
        			corCurRef->title = newTitle;
        		}
        	}

        	if (corYear) {
        		string actualYear = cpt.get<string>("abstracts-retrieval-response.coredata.prism:coverDate");
	        	std::vector<string> strs;
	        	boost::split(strs, actualYear, boost::is_any_of("-"));
    	    	string newYear = strs[0];

        		if (curref->year != newYear) {
        			corCurRef->year = newYear;
        		}
        	}

        	if (corSourceTitle) {
        		string newSourceTitle = cpt.get<string>("abstracts-retrieval-response.coredata.prism:publicationName");
        		if (curref->sourceTitle != newSourceTitle) {
	        		corCurRef->sourceTitle = newSourceTitle;
        		}
        	}

        	if (corVolume) {
        		string newVolume = cpt.get<string>("abstracts-retrieval-response.coredata.prism:volume");
        		if (curref->volume != newVolume) {
        			corCurRef->volume = newVolume;
        		}
        	}

        	if (corIssue) {
        		string newIssue = cpt.get<string>("abstracts-retrieval-response.coredata.prism:issueIdentifier");
        		if (curref->issue != newIssue) {
        			corCurRef->issue = newIssue;
        		}
        	}

        	if (corPageStart) {
        		string newPageStart = cpt.get<string>("abstracts-retrieval-response.coredata.prism:startingPage");
        		if (curref->pageStart != newPageStart) {
        			corCurRef->pageStart = newPageStart;
        		}
        	}

        	if (corPageEnd) {
        		string newPageEnd = cpt.get<string>("abstracts-retrieval-response.coredata.prism:endingPage");
        		if (curref->pageEnd != newPageEnd) {
        			corCurRef->pageEnd = newPageEnd;
        		}
        	}

        	if (corDoi) {
        		string newDoi = cpt.get<string>("abstracts-retrieval-response.coredata.prism:doi");
        		if (curref->doi != newDoi) {
        			corCurRef->doi = newDoi;
        		}
        	}

        	reflist.push_back(curref);
        	corRefList.push_back(corCurRef);
		}

		std::cout << "All DOI's processed" << std::endl;
		d.outputToCSV(reflist, corRefList);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return reflist;
}

