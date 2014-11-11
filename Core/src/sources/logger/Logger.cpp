#include "logger/Logger.h"

bool Susi::fileExists (const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

void Susi::rolloutHandler(const char* filename, std::size_t size) {
    std::string infile{filename};
    std::string outfile = infile;
    for(uint64_t i=0;(i+1)!=0;i++){
        if(!fileExists(infile+"."+std::to_string(i)+".xz")){
            outfile += ("."+std::to_string(i)+".xz");
            break;
        }
    }
    if(infile==outfile){
        std::cerr<<"you have 2^64 logfiles, I think something is wrong. (If every logfile has 1 MB then you have 17179869184 PB of logs, happy reading)"<<std::endl;
        return;
    }
    std::cout<<"compressing..."<<std::endl;
    compress_lzma(infile,outfile);
    std::cout<<"compressing ready!"<<std::endl;
}

/*
bool Susi::compress_zlib(std::string infilename, std::string outfilename){
    FILE *infile = fopen(infilename.c_str(), "rb");
    gzFile outfile = gzopen(outfilename.c_str(), "wb");
    if (!infile || !outfile) return false;

    char inbuffer[128];
    int num_read = 0;
    unsigned long total_read = 0, total_wrote = 0;
    while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0) {
       total_read += num_read;
       gzwrite(outfile, inbuffer, num_read);
    }
    fclose(infile);
    gzclose(outfile);
    return true;
}
*/


/* analogous to xz CLI options: -0 to -9 */
#define COMPRESSION_LEVEL 6
/* boolean setting, analogous to xz CLI option: -e */
#define COMPRESSION_EXTREME false
/* see: /usr/include/lzma/check.h LZMA_CHECK_* */
#define INTEGRITY_CHECK LZMA_CHECK_CRC64
#define IN_BUF_MAX  4096
#define OUT_BUF_MAX 4096
/* error codes */
#define RET_OK          0
#define RET_ERROR_INIT      1
#define RET_ERROR_INPUT     2
#define RET_ERROR_OUTPUT    3
#define RET_ERROR_COMPRESSION   4
bool Susi::compress_lzma(std::string infilename, std::string outfilename){
    FILE *in_file = fopen(infilename.c_str(), "rb");
    FILE *out_file = fopen(outfilename.c_str(), "wb");
    uint32_t preset = COMPRESSION_LEVEL | (COMPRESSION_EXTREME ? LZMA_PRESET_EXTREME : 0);
    lzma_check check = INTEGRITY_CHECK;
    lzma_stream strm = LZMA_STREAM_INIT; /* alloc and init lzma_stream struct */
    uint8_t in_buf [IN_BUF_MAX];
    uint8_t out_buf [OUT_BUF_MAX];
    size_t in_len;  /* length of useful data in in_buf */
    size_t out_len; /* length of useful data in out_buf */
    bool in_finished = false;
    bool out_finished = false;
    lzma_action action;
    lzma_ret ret_xz;
    int ret;

    ret = RET_OK;

    /* initialize xz encoder */
    ret_xz = lzma_easy_encoder (&strm, preset, check);
    if (ret_xz != LZMA_OK) {
        fprintf (stderr, "lzma_easy_encoder error: %d\n", (int) ret_xz);
        return RET_ERROR_INIT;
    }

    while ((! in_finished) && (! out_finished)) {
        /* read incoming data */
        in_len = fread (in_buf, 1, IN_BUF_MAX, in_file);
        if (feof (in_file)) {
            in_finished = true;
        }
        if (ferror (in_file)) {
            in_finished = true;
            ret = RET_ERROR_INPUT;
        }

        strm.next_in = in_buf;
        strm.avail_in = in_len;

        /* if no more data from in_buf, flushes the
           internal xz buffers and closes the xz data
           with LZMA_FINISH */
        action = in_finished ? LZMA_FINISH : LZMA_RUN;

        /* loop until there's no pending compressed output */
        do {
            /* out_buf is clean at this point */
            strm.next_out = out_buf;
            strm.avail_out = OUT_BUF_MAX;

            /* compress data */
            ret_xz = lzma_code (&strm, action);

            if ((ret_xz != LZMA_OK) && (ret_xz != LZMA_STREAM_END)) {
                fprintf (stderr, "lzma_code error: %d\n", (int) ret_xz);
                out_finished = true;
                ret = RET_ERROR_COMPRESSION;
            } else {
                /* write compressed data */
                out_len = OUT_BUF_MAX - strm.avail_out;
                fwrite (out_buf, 1, out_len, out_file);
                if (ferror (out_file)) {
                    out_finished = true;
                    ret = RET_ERROR_OUTPUT;
                }
            }
        } while (strm.avail_out == 0);
    }

    lzma_end (&strm);
    fclose(outfile);
    return ret;
}