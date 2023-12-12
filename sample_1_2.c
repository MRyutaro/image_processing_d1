#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * マクロ定義
 */
#define min(A, B) ((A) < (B) ? (A) : (B))
#define max(A, B) ((A) > (B) ? (A) : (B))

/*
 * 画像構造体の定義
 */
typedef struct
{
    int width;           /* 画像の横方向の画素数 */
    int height;          /* 画像の縦方向の画素数 */
    int maxValue;        /* 画素の値(明るさ)の最大値 */
    unsigned char *data; /* 画像の画素値データを格納する領域を指す */
                         /* ポインタ */
} image_t;

/*
 * 画素値データがint型の画像構造体の定義
 */
typedef struct
{
    int width;           /* 画像の横方向の画素数 */
    int height;          /* 画像の縦方向の画素数 */
    int minValue;        /* 画素の値(明るさ)の最小値 */
    int maxValue;        /* 画素の値(明るさ)の最大値 */
    unsigned int *data;           /* 画像の画素値データを格納する領域を指す */
                         /* ポインタ */
} int_image_t;

/*
 * パディングを加えた画像構造体の定義
 */
typedef struct
{
    int width;           /* パディングを加えた画像の横方向の画素数 */
    int height;          /* パディングを加えた画像の縦方向の画素数 */
    int maxValue;        /* 画素の値(明るさ)の最大値 */
    int padding_x;       /* パディングの横方向の画素数 */
    int padding_y;       /* パディングの縦方向の画素数 */
    unsigned char *data; /* パディングを加えた画像の画素値データを格納する領域を指す */
                         /* ポインタ */
} padding_image_t;

/*
 * カーネル構造体の定義
 */
typedef struct
{
    int width;           /* カーネルの横方向の画素数 */
    int height;          /* カーネルの縦方向の画素数 */
    int *data;           /* カーネルの画素値データを格納する領域を指す */
                         /* ポインタ */
} kernel_t;

/*======================================================================
 * このプログラムに与えられた引数の解析
 *======================================================================
 */
void parseArg(int argc, char **argv, FILE **infp, FILE **outfp)
{
    FILE *fp;

    /* 引数の個数をチェック */
    if (argc != 3)
    {
        goto usage;
    }

    *infp = fopen(argv[1], "rb"); /* 入力画像ファイルをバイナリモードで */
                                  /* オープン */

    if (*infp == NULL) /* オープンできない時はエラー */
    {
        fputs("Opening the input file was failend\n", stderr);
        goto usage;
    }

    *outfp = fopen(argv[2], "wb"); /* 出力画像ファイルをバイナリモードで */
                                   /* オープン */

    if (*outfp == NULL) /* オープンできない時はエラー */
    {
        fputs("Opening the output file was failend\n", stderr);
        goto usage;
    }

    return;

/* このプログラムの使い方の説明 */
usage:
    fprintf(stderr, "usage : %s <input pgm file> <output pgm file>\n", argv[0]);
    exit(1);
}

/*======================================================================
 * 画像構造体の初期化
 *======================================================================
 * 画像構造体 image_t *ptImage の画素数(width × height)、階調数
 * (maxValue)を設定し、画素値データを格納するのに必要なメモリ領域を確
 * 保する。
 */
void initImage(image_t *ptImage, int width, int height, int maxValue)
{
    ptImage->width = width;
    ptImage->height = height;
    ptImage->maxValue = maxValue;

    /* メモリ領域の確保 */
    ptImage->data = (unsigned char *)malloc(sizeof(unsigned char)*(width * height));

    if (ptImage->data == NULL) /* メモリ確保ができなかった時はエラー */
    {
        fputs("out of memory\n", stderr);
        exit(1);
    }

    return;
}

/*======================================================================
 * カーネル構造体の初期化
 *======================================================================
 */
void initKernel(kernel_t *ptKernel, int width, int height)
{
    ptKernel->width = width;
    ptKernel->height = height;

    /* メモリ領域の確保 */
    ptKernel->data = (int *)malloc(sizeof(int)*(width * height));

    if (ptKernel->data == NULL) /* メモリ確保ができなかった時はエラー */
    {
        fputs("out of memory\n", stderr);
        exit(1);
    }

    return;
}

/*======================================================================
 * パディングを加えた画像構造体の初期化
 *======================================================================
 */
void initPaddingImage(image_t *originalImage, padding_image_t *ptPaddingImage, int kernel_width, int kernel_height)
{
    int original_image_width = originalImage->width;
    int original_image_height = originalImage->height;

    /* パディングの大きさ */
    int padding_x = (kernel_width - 1) / 2;
    int padding_y = (kernel_height - 1) / 2;

    /* パディングを加えた画像のサイズ */
    int width = original_image_width + padding_x * 2;
    int height = original_image_height + padding_y * 2;
    int maxValue = originalImage->maxValue;

    ptPaddingImage->width = width;
    ptPaddingImage->height = height;
    ptPaddingImage->maxValue = maxValue;
    ptPaddingImage->padding_x = padding_x;
    ptPaddingImage->padding_y = padding_y;

    /* メモリ領域の確保 */
    ptPaddingImage->data = (unsigned char *)malloc(sizeof(unsigned char)*(width * height));

    if (ptPaddingImage->data == NULL) /* メモリ確保ができなかった時はエラー */
    {
        fputs("out of memory\n", stderr);
        exit(1);
    }

    return;
}

/*======================================================================
 * int型画像構造体の初期化
 *======================================================================
 * 画像構造体 int_image_t *ptImage の画素数(width × height)
 * を設定し、画素値データを格納するのに必要なメモリ領域を確保する。
 */
void initIntImage(int_image_t *ptImage, int width, int height)
{
    ptImage->width = width;
    ptImage->height = height;

    /* メモリ領域の確保 */
    ptImage->data = (unsigned int *)malloc(sizeof(unsigned int)*(width * height));

    if (ptImage->data == NULL) /* メモリ確保ができなかった時はエラー */
    {
        fputs("out of memory\n", stderr);
        exit(1);
    }

    return;
}

/*======================================================================
 * 文字列一行読み込み関数
 *======================================================================
 *   FILE *fp から、改行文字'\n'が表れるまで文字を読み込んで、char型の
 * メモリ領域 char *buf に格納する。1行の長さが n 文字以上の場合は、先
 * 頭から n-1 文字だけを読み込む。
 *   読み込んだ文字列の先頭が '#' の場合は、さらに次の行を読み込む。
 *   正常に読み込まれた場合は、ポインタ buf を返し、エラーや EOF (End
 * Of File) の場合は NULL を返す。
 */
char *readOneLine(char *buf, int n, FILE *fp)
{
    char *fgetsResult;

    do
    {
        fgetsResult = fgets(buf, n, fp);
    } while (fgetsResult != NULL && buf[0] == '#');
    /* エラーや EOF ではなく、かつ、先頭が '#' の時は、次の行 */
    /* を読み込む */

    return fgetsResult;
}

/*======================================================================
 * PGM-RAW フォーマットのヘッダ部分の読み込みと画像構造体の初期化
 *======================================================================
 *   PGM-RAW フォーマットの画像データファイル FILE *fp から、ヘッダ部
 * 分を読み込んで、その画像の画素数、階調数を調べ、その情報に従って、
 * 画像構造体 image_t *ptImage を初期化する。
 *   画素値データを格納するメモリ領域も確保し、この領域の先頭を指すポ
 * インタを ptImage->data に格納する。
 *
 * !! 注意 !!
 *   この関数は、ほとんどの場合、正しく動作するが、PGM-RAWフォーマット
 * の正確な定義には従っておらず、正しいPGM-RAWフォーマットのファイルに
 * 対して、不正な動作をする可能性がある。なるべく、本関数をそのまま使
 * 用するのではなく、正しく書き直して利用せよ。
 */
void readPgmRawHeader(FILE *fp, image_t *ptImage)
{
    int width, height, maxValue;
    char buf[128];

    /* マジックナンバー(P5) の確認 */
    if (readOneLine(buf, 128, fp) == NULL)
    {
        goto error;
    }
    if (buf[0] != 'P' || buf[1] != '5')
    {
        goto error;
    }

    /* 画像サイズの読み込み */
    if (readOneLine(buf, 128, fp) == NULL)
    {
        goto error;
    }
    if (sscanf(buf, "%d %d", &width, &height) != 2)
    {
        goto error;
    }
    if (width <= 0 || height <= 0)
    {
        goto error;
    }

    /* 最大画素値の読み込み */
    if (readOneLine(buf, 128, fp) == NULL)
    {
        goto error;
    }
    if (sscanf(buf, "%d", &maxValue) != 1)
    {
        goto error;
    }
    if (maxValue <= 0 || maxValue >= 256)
    {
        goto error;
    }

    /* 画像構造体の初期化 */
    initImage(ptImage, width, height, maxValue);

    return;

/* エラー処理 */
error:
    fputs("Reading PGM-RAW header was failed\n", stderr);
    exit(1);
}

/*======================================================================
 * PGM-RAWフォーマットの画素値データの読み込み
 *======================================================================
 *   入力ファイル FILE *fp から総画素数分の画素値データを読み込んで、
 * 画像構造体 image_t *ptImage の data メンバーが指す領域に格納する
 */
void readPgmRawBitmapData(FILE *fp, image_t *ptImage)
{
    if (fread(ptImage->data, sizeof(unsigned char),
              ptImage->width * ptImage->height, fp) != ptImage->width * ptImage->height)
    {
        /* エラー */
        fputs("Reading PGM-RAW bitmap data was failed\n", stderr);
        exit(1);
    }
}

/*======================================================================
 * パディングを加えた画像の初期化
 *======================================================================
 */
void setPaddingImageData(image_t *originalImage, padding_image_t *paddingImage, int kernel_width, int kernel_height)
{
    int original_image_width = originalImage->width;

    /* パディングの大きさ */
    int padding_x = paddingImage->padding_x;
    int padding_y = paddingImage->padding_y;

    /* パディングを加えた画像のサイズ */
    int padding_image_width = paddingImage->width;
    int padding_image_height = paddingImage->height;

    /* データのセット */
    for (int y = 0; y < padding_image_height; y++)
    {
        for (int x = 0; x < padding_image_width; x++)
        {
            if (x < padding_x || x >= padding_image_width - padding_x || y < padding_y || y >= padding_image_height - padding_y)
            {
                /* ゼロパディング */
                paddingImage->data[x + padding_image_width * y] = 0;
            }
            else
            {
                paddingImage->data[x + padding_image_width * y] = originalImage->data[(x - padding_x) + original_image_width * (y - padding_y)];
            }
        }
    }

    return;
}

/*======================================================================
 * 畳み込み演算
 *======================================================================
 */
int convolution(int x, int y, padding_image_t *paddingImage, kernel_t *kernel)
{
    int sum = 0;

    int kernel_width = kernel->width;
    int kernel_height = kernel->height;

    int half_kernel_width = (kernel_width - 1) / 2;
    int half_kernel_height = (kernel_height - 1) / 2;
    for (int j = 0; j < kernel_height; j++)
    {
        for (int i = 0; i < kernel_width; i++)
        {
            int paddingImage_pixel = paddingImage->data[(x + (i - half_kernel_width)) + paddingImage->width * (y + (j - half_kernel_height))];
            int kernel_pixel = kernel->data[i + kernel_width * j];
            sum += paddingImage_pixel * kernel_pixel;
        }
    }

    return sum;
}

/*======================================================================
 * [0, 255]に正規化した画像データのセット
 *======================================================================
 */
void setNormalizedImageData(int_image_t *tmpImage, image_t *resultImage)
{
    /* サイズが違ったらエラー */
    if (tmpImage->width != resultImage->width || tmpImage->height != resultImage->height)
    {
        fputs("tmpImage and resultImage are different size\n", stderr);
        exit(1);
    }

    int tmp_image_width = tmpImage->width;
    int tmp_image_height = tmpImage->height;
    int tmp_image_minValue = tmpImage->minValue;
    int tmp_image_maxValue = tmpImage->maxValue;
    int result_image_maxValue = resultImage->maxValue;

    printf("tmp_image: minValue=%d, maxValue=%d\n", tmp_image_minValue, tmp_image_maxValue);

    /* データのセット */
    for (int y = 0; y < tmp_image_height; y++)
    {
        for (int x = 0; x < tmp_image_width; x++)
        {
            int tmp_image_pixel = tmpImage->data[x + tmp_image_width * y];
            /* x'=255*(x-min)/(max-min) (x'の範囲[0, 255]) */
            int result_image_pixel = (int)(((double)(tmp_image_pixel - tmp_image_minValue) / (double)(tmp_image_maxValue - tmp_image_minValue)) * (double)result_image_maxValue);
            resultImage->data[x + tmp_image_width * y] = result_image_pixel;
        }
    }

    return;
}

/*======================================================================
 * フィルタリング(Prewittフィルタ+(3))
 *======================================================================
 */
void filteringImage(image_t *resultImage, image_t *originalImage)
{
    /* サイズが違ったらエラー */
    if (resultImage->width != originalImage->width || resultImage->height != originalImage->height)
    {
        fputs("resultImage and originalImage are different size\n", stderr);
        exit(1);
    }

    kernel_t kernel_x, kernel_y;
    padding_image_t paddingImage;
    int_image_t tmpImage;

    int original_image_width = originalImage->width;
    int original_image_height = originalImage->height;

    /* フィルタ */
    int kernel_width = 3;
    int kernel_height = 3;
    kernel_x.width = kernel_width;
    kernel_x.height = kernel_height;
    kernel_y.width = kernel_width;
    kernel_y.height = kernel_height;

    /* フィルタの初期化 */
    initKernel(&kernel_x, kernel_width, kernel_height);
    initKernel(&kernel_y, kernel_width, kernel_height);

    /* データのセット */
    int kernel_x_data[] = {
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1};
    int kernel_y_data[] = {
        -1, -1, -1,
        0, 0, 0,
        1, 1, 1};
    for (int i = 0; i < kernel_width * kernel_height; i++)
    {
        kernel_x.data[i] = kernel_x_data[i];
        kernel_y.data[i] = kernel_y_data[i];
    }
    /* 偶数ならエラー */
    if (kernel_width % 2 == 0 || kernel_height % 2 == 0)
    {
        fputs("kernel_width or kernel_height is even number\n", stderr);
        exit(1);
    }

    /* パディングを加えた画像の初期化 */
    initPaddingImage(originalImage, &paddingImage, kernel_width, kernel_height);
    /* パディングを加えた画像のデータのセット */
    setPaddingImageData(originalImage, &paddingImage, kernel_width, kernel_height);

    /* 値がint型のtmpImageの初期化 */
    initIntImage(&tmpImage, original_image_width, original_image_height);

    /* 各要素の確認 */
    printf("original_image: width=%d, height=%d, maxValue=%d\n", original_image_width, original_image_height, originalImage->maxValue);
    printf("kernel_x: width=%d, height=%d\n", kernel_x.width, kernel_x.height);
    printf("kernel_y: width=%d, height=%d\n", kernel_y.width, kernel_y.height);
    printf("padding_image: width=%d, height=%d, maxValue=%d, padding_x=%d, padding_y=%d\n", paddingImage.width, paddingImage.height, paddingImage.maxValue, paddingImage.padding_x, paddingImage.padding_y);
    printf("tmp_image: width=%d, height=%d\n", tmpImage.width, tmpImage.height);
    printf("result_image_before: width=%d, height=%d, maxValue=%d\n", resultImage->width, resultImage->height, resultImage->maxValue);

    int padding_x = paddingImage.padding_x;
    int padding_y = paddingImage.padding_y;

    int tmp_image_minValue = 255;
    int tmp_image_maxValue = 0;

    /* フィルタリング */
    for (int y = padding_y; y < original_image_height + padding_y; y++)
    {
        for (int x = padding_x; x < original_image_width + padding_x; x++)
        {
            /* 畳み込み演算 */
            int dfdx = convolution(x, y, &paddingImage, &kernel_x);
            int dfdy = convolution(x, y, &paddingImage, &kernel_y);
            unsigned int g = abs(dfdx) + abs(dfdy);

            /* データのセット */
            tmpImage.data[(x - padding_x) + original_image_width * (y - padding_y)] = (unsigned int)g;

            /* 最小値の更新 */
            if (tmp_image_minValue > g)
            {
                tmp_image_minValue = g;
            }
            /* 最大値の更新 */
            if (tmp_image_maxValue < g)
            {
                tmp_image_maxValue = g;
            }
        }
    }

    /* tmpImageの最小値をセット */
    tmpImage.minValue = tmp_image_minValue;
    /* tmpImageの最大値をセット */
    tmpImage.maxValue = tmp_image_maxValue;
    /* [0, 255]に正規化したものをresultImageにセット */
    setNormalizedImageData(&tmpImage, resultImage);

    /* 計算結果の確認 */
    printf("result_image_after: width=%d, height=%d, maxValue=%d\n", resultImage->width, resultImage->height, resultImage->maxValue);

    return;
}

/*======================================================================
 * PGM-RAW フォーマットのヘッダ部分の書き込み
 *======================================================================
 *   画像構造体 image_t *ptImage の内容に従って、出力ファイル FILE *fp
 * に、PGM-RAW フォーマットのヘッダ部分を書き込む。
 */
void writePgmRawHeader(FILE *fp, image_t *ptImage)
{
    /* マジックナンバー(P5) の書き込み */
    if (fputs("P5\n", fp) == EOF)
    {
        goto error;
    }

    /* 画像サイズの書き込み */
    if (fprintf(fp, "%d %d\n", ptImage->width, ptImage->height) == EOF)
    {
        goto error;
    }

    /* 画素値の最大値を書き込む */
    if (fprintf(fp, "%d\n", ptImage->maxValue) == EOF)
    {
        goto error;
    }

    return;

error:
    fputs("Writing PGM-RAW header was failed\n", stderr);
    exit(1);
}

/*======================================================================
 * PGM-RAWフォーマットの画素値データの書き込み
 *======================================================================
 *   画像構造体 image_t *ptImage の内容に従って、出力ファイル FILE *fp
 * に、PGM-RAW フォーマットの画素値データを書き込む
 */
void writePgmRawBitmapData(FILE *fp, image_t *ptImage)
{
    if (fwrite(ptImage->data, sizeof(unsigned char),
               ptImage->width * ptImage->height, fp) != ptImage->width * ptImage->height)
    {
        /* エラー */
        fputs("Writing PGM-RAW bitmap data was failed\n", stderr);
        exit(1);
    }
}

/*
 * メイン
 */
int main(int argc, char **argv)
{
    image_t originalImage, resultImage;
    FILE *infp, *outfp;

    /* 引数の解析 */
    parseArg(argc, argv, &infp, &outfp);

    /* 元画像の画像ファイルのヘッダ部分を読み込み、画像構造体を初期化 */
    /* する */
    readPgmRawHeader(infp, &originalImage);

    /* 元画像の画像ファイルのビットマップデータを読み込む */
    readPgmRawBitmapData(infp, &originalImage);

    /* 結果画像の画像構造体を初期化する。画素数、階調数は元画像と同じ */
    initImage(
        &resultImage,
        originalImage.width,
        originalImage.height,
        originalImage.maxValue);

    /* フィルタリング */
    filteringImage(&resultImage, &originalImage);

    /* 画像ファイルのヘッダ部分の書き込み */
    writePgmRawHeader(outfp, &resultImage);

    /* 画像ファイルのビットマップデータの書き込み */
    writePgmRawBitmapData(outfp, &resultImage);

    return 0;
}
