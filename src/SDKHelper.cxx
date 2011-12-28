#include "config.h"
#include "SDKHelper.hxx"
#include <boost/make_shared.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <util/common/StringUtil.hxx>
#include <HttpEngine/IHttpEngineCommon.hxx>

#include "strconv.h"
#include "Urlcode.h"
#include "IWeiboMethod.hxx"

using namespace weibo;

struct tagMethodURLElement
{
	int option;
	const char* url;
};

#if defined(INTERNAL_INTERFACE_USEABLE)
#	include "Internal/SDKInternURL.hxx"
#endif //

static const struct tagMethodURLElement gMethodURLElements[] = 
{
	{ WBOPT_NONE, "" },
	{ WBOPT_GET_STATUSES_PUBLIC_TIMELINE             , "/statuses/public_timeline"  }, //��ȡ���¸��µĹ���΢����Ϣ 
	{ WBOPT_GET_STATUSES_FRIENDS_TIMELINE            , "/statuses/friends_timeline" }, //��ȡ��ǰ�û�����ע�û�������΢����Ϣ (����: statuses/home_timeline) 
	{ WBOPT_GET_STATUSES_HOME_TIMELINE               , "/statuses/home_timeline" }, //��ȡ��ǰ��¼�û���������ע�û�������΢����Ϣ
	{ WBOPT_GET_STATUSES_USER_TIMELINE               , "/statuses/user_timeline"}, //��ȡ�û�������΢����Ϣ�б� 
	{ WBOPT_GET_STATUSES_TIMELINE_BATCH               , "/statuses/timeline_batch" }, //�������ָ����ĳһ���û���timeline
	{ WBOPT_GET_STATUSES_REPOST_TIMELINE             , "/statuses/repost_timeline" }, //����һ��΢��������n��ת��΢����Ϣ
	{ WBOPT_GET_STATUSES_REPOST_BYME                 , "/statuses/repost_by_me" }, //�����û�ת��������n��΢����Ϣ
	{ WBOPT_GET_STATUSES_MENTIONS                    , "/statuses/mentions" },//��ȡ@��ǰ�û���΢���б� 
	{ WBOPT_GET_STATUSES_SHOW                        , "/statuses/show" },//����ID��ȡ����΢����Ϣ
	{ WBOPT_GET_STATUSES_SHOW_BATCH                  , "/statuses/show_batch" },//������ȡ΢����Ϣ
	{ WBOPT_GET_STATUSES_QUERYMID                    , "/statuses/querymid" },//ͨ��id��ȡmid
	{ WBOPT_GET_STATUSES_QUERYID                     , "/statuses/queryid" },//ͨ��mid��ȡid
	{ WBOPT_GET_STATUSES_REPOST_DAILY                , "/statuses/hot/repost_daily"    },//���췵������ת����
	{ WBOPT_GET_STATUSES_REPOST_WEEKLY               , "/statuses/hot/repost_weekly"   },//���ܷ�������ת����
	{ WBOPT_GET_STATUSES_HOT_COMMENTS_DAILY          , "/statuses/hot/comments_daily"  },//���췵���������۰�
	{ WBOPT_GET_STATUSES_HOT_COMMENTS_WEEKLY         , "/statuses/hot/comments_weekly" },//���ܷ����������۰�

	{ WBOPT_POST_STATUSES_REPOST                , "/statuses/repost"  },//ת��һ��΢����Ϣ
	{ WBOPT_POST_STATUSES_DESTROY               , "/statuses/destroy" },//ɾ��΢��������Ϣ
	{ WBOPT_POST_STATUSES_UPDATE                , "/statuses/update"  },//����һ��΢����Ϣ
	{ WBOPT_POST_STATUSES_UPLOAD_URL_TEXT       , "/statuses/upload_url_text" },//����һ��΢����ͬʱָ���ϴ���ͼƬ��ͼƬurl
	{ WBOPT_POST_STATUSES_UPLOAD                , "/statuses/upload"  },//����һ��΢�����ϴ�ͼƬ������һ��΢����Ϣ

	{ WBOPT_GET_COMMENTS_SHOW          , "/comments/show" }, //��ȡĳ��΢���������б�
	{ WBOPT_GET_COMMENTS_BYME          , "/comments/by_me" }, //�ҷ����������б�
	{ WBOPT_GET_COMMENTS_TOME          , "/comments/to_me" }, //���յ��������б�
	{ WBOPT_GET_COMMENTS_TIMELINE      , "/comments/timeline" }, //��ȡ��ǰ�û����ͼ��յ��������б�
	{ WBOPT_GET_COMMENTS_MENTIONS      , "/comments/mentions" }, //@���ҵ�����
	{ WBOPT_GET_COMMENTS_SHOWBATCH     , "/comments/show_batch" }, //@������ȡ��������

	{ WBOPT_POST_COMMENTS_CREATE        , "/comments/create" }, //����һ��΢��
	{ WBOPT_POST_COMMENTS_DESTROY       , "/comments/destroy" }, //ɾ��һ������
	{ WBOPT_POST_COMMENTS_DESTROY_BATCH , "/comments/destroy_batch" }, //����ɾ������
	{ WBOPT_POST_COMMENTS_REPLY         , "/comments/reply" }, //�ظ�һ������

	{ WBOPT_GET_DIRECTMESSAGES,				"/direct_messages"}, //��ȡ��ǰ�û�����˽���б�
	{ WBOPT_GET_DIRECTMESSAGES_SENT,			"/direct_messages/sent"}, //��ȡ��ǰ�û����͵�����˽���б�
	{ WBOPT_GET_DIRECTMESSAGES_USER_LIST,    "/direct_messages/user_list"}, //��ȡ˽�������û��б�
	{ WBOPT_GET_DIRECTMESSAGES_CONVERSATION, "/direct_messages/conversation"}, //��ȡ��ָ���û�������˽���б�
	{ WBOPT_GET_DIRECTMESSAGES_SHOW_BATCH,	"/direct_messages/show_batch"}, //������ȡ˽������
	{ WBOPT_GET_DIRECTMESSAGES_IS_CAPABLE,	"/direct_messages/is_capable"}, //�ж��Ƿ���Ը��Է���˽��
	{ WBOPT_POST_DIRECTMESSAGES_NEW,			"/direct_messages/new"}, //����һ��˽��
	{ WBOPT_POST_DIRECTMESSAGES_DESTORY,		"/direct_messages/destroy"}, //ɾ��һ��˽��
	{ WBOPT_POST_DIRECTMESSAGES_DESTORY_BATCH,"/direct_messages/destroy_batch"},	//����ɾ��˽��

	{ WBOPT_GET_USERS_SHOW,					"/users/show"}, //��ȡ�û�����
	{ WBOPT_GET_USERS_DOMAIN_SHOW,			"/users/domain_show"}, //ͨ������������ȡ�û���Ϣ
	{ WBOPT_GET_USERS_SHOW_BATCH,			"/users/show_batch"}, //������ȡ�û���Ϣ

	{ WBOPT_GET_ACCOUNT_PROFILE_BASIC, "/account/profile/basic" }, //��ȡ�û�������Ϣ
	{ WBOPT_GET_ACCOUNT_PROFILE_EDUCATION, "/account/profile/education" }, //��ȡ������Ϣ
	{ WBOPT_GET_ACCOUNT_PROFILE_EDUCATION_BATCH, "/account/profile/education_batch" }, //������ȡ������Ϣ
	{ WBOPT_GET_ACCOUNT_PROFILE_CAREER, "/account/profile/career" }, //��ȡְҵ��Ϣ
	{ WBOPT_GET_ACCOUNT_PROFILE_CAREER_BATCH, "/account/profile/career_batch" }, //������ȡְҵ��Ϣ
	{ WBOPT_GET_ACCOUNT_GET_PRIVACY, "/account/get_privacy" }, //��ȡ��˽������Ϣ
	{ WBOPT_GET_ACCOUNT_PROFILE_SCHOOL_LIST, "/account/profile/school_list" }, //��ȡ����ѧУ�б�
	{ WBOPT_GET_ACCOUNT_RATE_LIMIT_STATUS, "/account/rate_limit_status" }, //��ȡ��ǰ�û�API����Ƶ������
	{ WBOPT_GET_ACCOUNT_GET_UID, "/account/get_uid" }, //OAuth��Ȩ֮���ȡ�û�UID�������൱�ھɰ�ӿڵ�account/verify_credentials��

	{ WBOPT_POST_ACCOUNT_PROFILE_BASIC_UPDATE , "/account/profile/basic_update" }, // �����û�������Ϣ
	{ WBOPT_POST_ACCOUNT_PROFILE_EDU_UPDATE, "/account/profile/edu_update" }, // �����û�������Ϣ
	{ WBOPT_POST_ACCOUNT_PROFILE_EDU_DESTROY , "/account/profile/edu_destroy" }, // ɾ���û�������Ϣ
	{ WBOPT_POST_ACCOUNT_PROFILE_CAR_UPDATE , "/account/profile/car_update" }, // �����û�ְҵ��Ϣ
	{ WBOPT_POST_ACCOUNT_PROFILE_CAR_DESTROY , "/account/profile/car_destroy" }, // ɾ���û�ְҵ��Ϣ
	{ WBOPT_POST_ACCOUNT_AVATAR_UPLOAD , "/account/avatar/upload" }, // �ϴ�ͷ��
	{ WBOPT_POST_ACCOUNT_UPDATE_PRIVACY , "/account/update_privacy" }, // ������˽����

	{ WBOPT_GET_FRIENDSHIPS_FRIENDS, "/friendships/friends"}, //��ȡ�û��Ĺ�ע�б�
	{ WBOPT_GET_FRIENDSHIPS_IN_COMMON, "/friendships/friends/in_common"}, //��ȡ��ͬ��ע���б�ӿ�
	{ WBOPT_GET_FRIENDSHIPS_BILATERAL, "/friendships/friends/bilateral"}, //��ȡ˫���ע�б�
	{ WBOPT_GET_FRIENDSHIPS_BILATERAL_IDS, "/friendships/friends/bilateral/ids"}, //��ȡ˫���עID�б�
	{ WBOPT_GET_FRIENDSHIPS_FRIENDS_IDS, "/friendships/friends/ids"}, //��ȡ�û���ע����uid�б�
	{ WBOPT_GET_FRIENDSHIPS_FRIENDS_REMARK_BATCH, "/friendships/friends/remark_batch"},	//������ȡ��ע
	{ WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS, "/friendships/followers"}, //��ȡ�û���˿�б�ÿ����˿������һ��΢��
	{ WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS_IDS, "/friendships/followers/ids"}, //��ȡ�û���˿����uid�б�
	{ WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS_ACTIVE,"/friendships/followers/active"},		//��ȡ�û����ʷ�˿�б��ϵ��
	{ WBOPT_GET_FRIENDSHIPS_CHAIN_FOLLOWERS, "/friendships/friends_chain/followers"},//��ȡ�ҵĹ�ע���й�ע��ָ���û��ķ���
	{ WBOPT_GET_FRIENDSHIPS_SHOW, "/friendships/show"}, //��ȡ�����û���ϵ����ϸ���
	{ WBOPT_POST_FRIENDSHIPS_CREATE, "/friendships/create"}, //��עĳ�û�
	{ WBOPT_POST_FRIENDSHIPS_CREATE_BATCH, "/friendships/create_batch"}, //������ע�û�
	{ WBOPT_POST_FRIENDSHIPS_DESTROY, "/friendships/destroy"}, //ȡ����עĳ�û�	
	{ WBOPT_POST_FRIENDSHIPS_FOLLOWERS_DESTORY,"/friendships/followers/destroy"}, //�Ƴ���˿
	{ WBOPT_POST_FRIENDSHIPS_REMARK_UPDATE, "/friendships/remark/update"}, //���¹�ע�˱�ע

	{ WBOPT_GET_FAVORITES, "/favorites" }, //��ȡ��ǰ��¼�û����ղ��б�
	{ WBOPT_GET_FAVORITES_SHOW, "/favorites/show" }, //��ȡ�����ղ���Ϣ
	{ WBOPT_GET_FAVORITES_BY_TAGS, "/favorites/by_tags" }, //��ȡ��ǰ�û�ĳ����ǩ�µ��ղ��б�
	{ WBOPT_GET_FAVORITES_TAGS, "/favorites/tags" }, //��ǰ��¼�û����ղر�ǩ�б�
	{ WBOPT_POST_FAVORITES_CREATE, "/favorites/create" }, //����ղ�
	{ WBOPT_POST_FAVORITES_DESTROY, "/favorites/destroy" }, //ɾ���ղ�
	{ WBOPT_POST_FAVORITES_DESTROY_BATCH, "/favorites/destroy_batch" }, //����ɾ���ղ�
	{ WBOPT_POST_FAVORITES_TAGS_UPDATE, "/favorites/tags/update" }, //�����ղر�ǩ
	{ WBOPT_POST_FAVORITES_TAGS_UPDATE_BATCH, "/favorites/tags/update_batch" }, //���µ�ǰ�û������ղ��µ�ָ����ǩ
	{ WBOPT_POST_FAVORITES_TAGS_DESTROY_BATCH, "/favorites/tags/destroy_batch" }, //ɾ����ǰ�û������ղ��µ�ָ����ǩ

	{ WBOPT_GET_TRENDS, "/trends" }, //��ȡĳ�˻���
	{ WBOPT_GET_TRENDS_STATUSES, "/trends/statuses" }, //��ȡĳһ�����µ�΢��
	{ WBOPT_GET_TRENDS_HOURLY, "/trends/hourly" }, //�������һСʱ�ڵ����Ż���
	{ WBOPT_GET_TRENDS_DAILY, "/trends/daily" }, //�������һ���ڵ����Ż���
	{ WBOPT_GET_TRENDS_WEEKLY, "/trends/weekly" }, //�������һ���ڵ����Ż���
	{ WBOPT_POST_TRENDS_FOLLOW, "/trends/follow" }, //��עĳ����
	{ WBOPT_POST_TRENDS_DESTROY, "/trends/destroy" }, //ȡ����ע��ĳһ������

	{ WBOPT_GET_TAGS, "/tags" }, //����ָ���û��ı�ǩ�б�
	{ WBOPT_GET_TAGS_TAGS_BATCH, "/tags/tags_batch" }, //������ȡ�û���ǩ
	{ WBOPT_GET_TAGS_SUGGESTIONS, "/tags/suggestions" }, //����ϵͳ�Ƽ��ı�ǩ�б�
	{ WBOPT_POST_TAGS_CREATE, "/tags/create" }, //����û���ǩ
	{ WBOPT_POST_TAGS_DESTROY, "/tags/destroy" }, //ɾ���û���ǩ
	{ WBOPT_POST_TAGS_DESTROY_BATCH, "/tags/destroy_batch" }, //����ɾ���û���ǩ

	{ WBOPT_GET_SEARCH_SUGGESTIONS_USERS, "/search/suggestions/users" },//���û���������
	{ WBOPT_GET_SEARCH_SUGGESTIONS_STATUSES, "/search/suggestions/statuses" },//��΢����������
	{ WBOPT_GET_SEARCH_SUGGESTIONS_SCHOOLS, "/search/suggestions/schools" },//��ѧУ��������
	{ WBOPT_GET_SEARCH_SUGGESTIONS_COMPANIES, "/search/suggestions/companies"},//�ѹ�˾��������
	{ WBOPT_GET_SEARCH_SUGGESTIONS_APPS, "/search/suggestions/apps" },//��Ӧ����������
	{ WBOPT_GET_SEARCH_SUGGESTIONS_AT_USERS, "/search/suggestions/at_users"},//@��������
	{ WBOPT_GET_SEARCH_SUGGESTIONS_INTEGRATE, "/search/suggestions/integrate"}, //�ۺ���������
	{ WBOPT_GET_SEARCH_STATUSES, "/search/statuses"}, //΢������ 
	{ WBOPT_GET_SEARCH_GEO_STATUSES, "/search/geo/statuses"},//����ָ����Χ�ڵ�΢��
	{ WBOPT_GET_SEARCH_USERS, "/search/users"}, //�����û�

	{ WBOPT_GET_SUGGESTIONS_FAVORITES_HOT, "/suggestions/favorites/hot" }, //�����ղ�
	{ WBOPT_GET_SUGGESTIONS_USERS_HOT, "/suggestions/users/hot" }, //��ȡϵͳ�Ƽ��û�
	{ WBOPT_GET_SUGGESTIONS_USERS_MAY_INTERESTED, "/suggestions/users/may_interested" }, //��ȡ�û����ܸ���Ȥ����
	{ WBOPT_POST_SUGGESTIONS_USERS_NOT_INTERESTED, "/suggestions/users/not_interested" }, //������Ȥ����
	{ WBOPT_GET_SUGGESTIONS_USERS_BY_STATUS, "/suggestions/users/by_status" }, //����΢�������Ƽ��û�

	{ WBOPT_GET_SHORT_URL_SHORTEN, "/short_url/shorten" }, //��һ������������ת���ɶ�����
	{ WBOPT_GET_SHORT_URL_EXPAND, "/short_url/expand" }, //��һ�����������ӻ�ԭ��ԭʼ�ĳ�����
	{ WBOPT_GET_SHORT_URL_SHARE_COUNTS, "/short_url/share/counts" }, //ȡ��һ����������΢���ϵ�΢��������������ԭ����ת����΢����
	{ WBOPT_GET_SHORT_URL_SHARE_STATUSES, "/short_url/share/statuses" }, //ȡ�ð���ָ�����������ӵ�����΢������
	{ WBOPT_GET_SHORT_URL_COMMENT_COUNTS, "/short_url/comment/counts" }, //ȡ��һ����������΢���ϵ�΢��������
	{ WBOPT_GET_SHORT_URL_COMMENT_COMMENTS, "/short_url/comment/comments" }, //ȡ�ð���ָ�����������ӵ�����΢����������
};

#define IN_BETWEEN(begin, end, optionId) \
	(begin <= methodOption && end >= methodOption)

void SDKHelper::getMethodURLFromOption(unsigned int methodOption, std::string &baseURL)
{
	const int counts = sizeof(gMethodURLElements)/sizeof(gMethodURLElements[0]);

	if (IN_BETWEEN(WBOPT_GET_SHORT_URL_SHORTEN, 
		WBOPT_GET_SHORT_URL_COMMENT_COMMENTS, methodOption))
	{
		baseURL = "https://api.t.sina.com.cn";
	}
	else
	{
		baseURL = "https://api.weibo.com/2";
	}

	bool bFound = false;
	for (int i = 0; i < counts; ++ i)
	{
		if (gMethodURLElements[i].option == methodOption)
		{
			bFound = true;
			baseURL += gMethodURLElements[i].url;
			break;
		}
	}

#if defined(INTERNAL_INTERFACE_USEABLE)

	if (!bFound)
	{
		if (IN_BETWEEN(WBOPT_GET_REMIND_UNREAD_COUNT, 
			WBOPT_POST_REMIND_CLEAR_COUNT, optionId))
		{
			baseURL = "https://api.t.sina.com.cn";
		}

		for (int i = 0; i < counts; ++ i)
		{
			if (gInternalMethodURLElements[i].option == methodOption)
			{
				baseURL += gInternalMethodURLElements[i].url;
				break;
			}
		}
	}

#endif //INTERNAL_INTERFACE_USEABLE
}

int SDKHelper::convertEngineErrorToSDKError(const int code)
{
	if (code == 0)
	{
		return WERR_OK;
	}

	switch(code)
	{
	case httpengine::HE_PROTOCOL_ERROR: return WERR_PROTOCOL;
	case httpengine::HE_CURL_ERROR: return WERR_CURL; 

	default:
		break;
	}
	return WERR_UNKNOW;
}

enum eParamFormat
{
	ParamFMT = 0,
	ParamFMT_UTF8 = 0x01,
	ParamFMT_URLENCODE = 0x02,
};

int getParamnameFormat(int paramformat)
{
	int paramf = 0;

	if (paramformat & ParamEncodedName)
	{
		paramf |= ParamFMT_URLENCODE;
	}

	if (paramformat & ParamUTF8Name) 
	{
		paramf |= ParamFMT_UTF8;
	}
	return paramf;
}

int getParamvalueFormat(int  paramformat)
{
	int paramf = 0;

	if (paramformat & ParamEncodedValue)
	{
		paramf |= ParamFMT_URLENCODE;
	}

	if(paramformat & ParamUTF8Value)
	{
		paramf |= ParamFMT_UTF8;
	}
	return paramf;
}

void setParamFormat(char* param, const char* paramval, int paramformat)
{
#if 0
	int vallen= 0;
	char* outstr = NULL;
	char* urlenc = NULL;

	if (paramformat & ParamFMT_UTF8)
	{
		if (lo_C2Utf8(&outstr, paramval) <= 0)
		{
			outstr = "";
		}
	}
	else
	{
		outstr = (char*)paramval;
	}

	if (paramformat & ParamFMT_URLENCODE)
	{		
		vallen = lo_UrlEncodeA(&urlenc , outstr , -1 );
		if ( vallen <= 0 )
		{
			if ( outstr != (char*)param && outstr ) 
			{
				free( outstr );
			}
			return ;
		}
		if (outstr != (char*)param && outstr)
		{
			free(outstr);
		}
	}
	else
	{
		urlenc = outstr;
	}
	strcat(param, urlenc);
	if (urlenc != (char*)paramval && urlenc)
	{
		free(urlenc);
	}
#else

	if (!paramval || paramval == '\0')
	{
		// Error : param value is null!
		return ;
	}

	char* resultStr = NULL;
	int resultLength = 0;

	// Need utf8 convert.
	if (paramformat & ParamFMT_UTF8)
	{
		resultLength = lo_C2Utf8(&resultStr , paramval);
	}
	else
	{
		resultStr = (char*)paramval;
	}

	if ((paramformat & ParamFMT_URLENCODE) && (resultStr && *resultStr != '\0'))
	{
		char* urlEncodeStr = NULL;
		const int urlEncodeLength = lo_UrlEncodeA(&urlEncodeStr, resultStr, -1);
		if (urlEncodeLength > 0 && urlEncodeStr)
		{
			if (resultLength > 0)
			{
				free(resultStr);	
			}
			resultStr = urlEncodeStr;
			resultLength = urlEncodeLength;
		}
	}

	if (resultStr && *resultStr != '\0')
	{
		strcat(param, resultStr);
		if (resultLength > 0)
		{
			free(resultStr);
		}
	}

#endif //0
}

void SDKHelper::setParam(char* param, const char *paramname, const char *paramval, int paramformat)
{
	if (!paramval || *paramval == '\0')
	{
		return;
	}
	setParamFormat(param, paramname, getParamnameFormat(paramformat));
	strcat(param, "=");
	setParamFormat(param, paramval, getParamvalueFormat(paramformat));
}

void SDKHelper::setIntParam(char * param, const char * paramName, const long long paramval, bool forceAdd)
{
	char val[16] = { 0 };
	if (!forceAdd && paramval <= 0)
	{
		return ;
	}
	sprintf(val, "%lld", paramval);
	setParam(param, paramName, (const char *)val, 0);
}

WeiboRequestPtr SDKHelper::makeRequest(unsigned int methodOption, char *addtionParam, const eWeiboRequestFormat reqformat
									   , const httpengine::HttpMethod method, const char* appkey, const char* accessToken, const UserTaskInfo* pTask)
{
	WeiboRequestPtr requestPtr = boost::make_shared<WeiboRequest>();
	requestPtr->mOptionId = methodOption;
	requestPtr->mHttpMethod = method;

	// base url
	std::string baseURL;
	SDKHelper::getMethodURLFromOption(methodOption, baseURL);

	// format 
	if (WRF_JSON == reqformat)
	{
		baseURL += ".json";
	}
	else if (WRF_XML == reqformat)
	{
		baseURL += ".xml";
	}

	// addtional param 
	if (addtionParam && *addtionParam != '\0')
	{
		if (std::string::npos == baseURL.find_first_of('?'))
		{
			addtionParam[0] = '?';
		}
		baseURL += addtionParam;
	}

	// Build request url and post args, from method.
	makeRequestURL(requestPtr->mURL, requestPtr->mPostArg
		, baseURL.c_str(), (method != httpengine::HM_GET), appkey, accessToken);

	if (pTask)
	{
		memcpy(&(requestPtr->mTaskInfo), pTask, sizeof(UserTaskInfo));
	}
	return requestPtr;
}


void SDKHelper::makeRequestURL(std::string &outURL, std::string &outParam, const char* baseURL
							   , bool isPost, const char* appkey, const char* accessToken)
{
	std::string url = Util::StringUtil::getNotNullString(baseURL);
	if (!url.empty())
	{
		if (isPost)
		{
			std::vector<std::string> vec;
			boost::split(vec, url, boost::is_any_of("?"));
			std::vector<std::string>::iterator it = vec.begin();
			outURL = *it;
			++ it;
			if (it != vec.end())
			{
				outParam = *it;
			}

			outParam += "&access_token=";
			outParam += Util::StringUtil::getNotNullString(accessToken);
		}
		else
		{
			outURL = url;
			outURL += (std::string::npos != outURL.find_first_of('?') ? "&source=" : "?source=" );
			outURL += Util::StringUtil::getNotNullString(appkey);

			outURL += "&access_token=";
			outURL += Util::StringUtil::getNotNullString(accessToken);
		}
	}
}

void SDKHelper::makeVariableParams(char *outParam, const int length, VariableParams* var)
{
	if (!var)
	{
		return ;
	}
	// based variable
	SDKHelper::setIntParam(outParam, "&since_id", var->since_id);
	SDKHelper::setIntParam(outParam, "&max_id", var->max_id);
	SDKHelper::setIntParam(outParam, "&count", var->count);
	SDKHelper::setIntParam(outParam, "&page", var->page);
	SDKHelper::setIntParam(outParam, "&base_app", var->base_app);
	SDKHelper::setIntParam(outParam, "&feature", var->feature);
	SDKHelper::setIntParam(outParam, "&is_encoded", var->is_encoded);
	SDKHelper::setIntParam(outParam, "&cursor", var->cursor);

	// variable for statuses
	SDKHelper::setParam(outParam, "&simplify", var->simplify.c_str(), PARAM_ENCODE_UTF8);
	SDKHelper::setIntParam(outParam, "&filter_by_source", var->filter_by_source);
	SDKHelper::setIntParam(outParam, "&filter_by_type", var->filter_by_type);
	SDKHelper::setIntParam(outParam, "&filter_by_author", var->filter_by_author);
	SDKHelper::setIntParam(outParam, "&trim_user", var->trim_user);
	SDKHelper::setIntParam(outParam, "&trim_status", var->trim_status);
}

void SDKHelper::makeIDParams(char *outParam, const int length, const ID *usrId)
{
	if (!usrId)
	{
		return ;
	}

	if (usrId->keyName.empty())
	{
		if (ID::IDT_ID == usrId->idType)
		{
			SDKHelper::setParam(outParam, "&uid", usrId->id.c_str(), ParamEncodedValue);
		}
		else if (ID::IDT_SCREENNAME == usrId->idType)
		{
			SDKHelper::setParam(outParam, "&screen_name", usrId->id.c_str(), ParamEncodedValue);
		}
	}
	else
	{
		std::string key = "&";
		key += usrId->keyName;
		SDKHelper::setParam(outParam, key.c_str(), usrId->id.c_str(), PARAM_ENCODE_UTF8);
	}
}
