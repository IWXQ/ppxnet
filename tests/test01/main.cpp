#include "ppx_net.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>


using namespace ppx;

std::atomic<bool> over;
std::atomic<ppx::net::FileTransferBase::Status> g_state;

void StatusCB(std::string filePath, ppx::net::FileTransferBase::Status state, std::string reason, int64_t used_millsec) {
    g_state = state;

    if (g_state == ppx::net::FileTransferBase::Status::Finished || g_state == ppx::net::FileTransferBase::Status::Failed) {
        over = true;
    }
}

void ProgressCB(std::string filePath, int64_t total, int64_t transfered) {
    static double last_per = 0.f;
    if (total > 0) {
        double per = ((double)transfered / (double)total * 100.0);
        if (per - last_per > 0.1f || total == transfered) {
            last_per = per;
            printf("%%%.1f\n", per);
        }
    }
    else {
        printf("--\n");
    }
}


void UploadSteambox() {


	std::string post_data = "{ \
		\"action\" : \"1\", \
		\"channel\" : \"front\", \
		\"ctype\" : 1, \
		\"cuid\" : \"234159078117238084\", \
		\"data\" : \"{\"ver\":\"2.1.0.1\",\"source\":\"auto_start\"}\"}";

	std::thread s = std::thread([post_data]() {
		net::HttpRequest request;
		base::BufferQueue rsp;
		request.SetConnectTimeoutMS(2000);
		request.SetReadTimeoutMS(1000);
		std::vector<std::string> headers;
		headers.push_back("Content-Type: application/json");

		int retry = 0;
		while (++retry <= 3) {
			int ret = request.Post("https://api.steamboxs.com/api/clientInfo", post_data.c_str(), post_data.length(), rsp, &headers);

			char* pRsp = NULL;
			int64_t rspLen = rsp.ToOneBuffer(&pRsp);
			if (rspLen < 0) {
				return;
			}

			std::string strRsp;
			strRsp.assign(pRsp, rspLen);

			free(pRsp);

			PPX_LOG(LS_INFO) << "Behavior UploadSteambox, post_data=" << post_data << ", rsp=" << strRsp;
			if (ret == 0)
				break;
		}
	});
	s.detach();
}


int main() {
	UploadSteambox();
	UploadSteambox();
	UploadSteambox();
	getchar();

    int thread_num = 10;
    bool interruption_resuming =  true ;
    std::string url = "https://dl.360safe.com/se/360se_setup.exe";
    std::string md5 = "620156055dfb6b20099593b7027b2ade";

    PPX_LOG(LS_INFO) << "**** [" << url << "] thread_num = " << thread_num << ", interruption_resuming = " << interruption_resuming;
    ppx::net::FileDownload fileDownload;
    fileDownload.SetUrl(url);
    fileDownload.SetFileDir("D:\\");
    fileDownload.SetFileName("test");
    fileDownload.SetFileExt(".exe");
    fileDownload.SetThreadNum(thread_num);
    fileDownload.SetFileMd5(md5);
    fileDownload.SetProgressInterval(100);
    fileDownload.SetInterruptionResuming(interruption_resuming);
    fileDownload.SetStatusCallback(std::bind(StatusCB, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    fileDownload.SetProgressCallback(std::bind(ProgressCB, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    fileDownload.Start(false);

    //while ( !over ) {
    //    Sleep(100);
    //}

    if ( g_state == ppx::net::FileTransferBase::Status::Finished ) {
        PPX_LOG(LS_INFO) << "**** [" << url << "] OK";
    }
    else {
        PPX_LOG(LS_INFO) << "**** [" << url << "] FAILED";
    }

    PPX_LOG(LS_INFO) << "**** TEST END";

    getchar();
    return 0;
}
