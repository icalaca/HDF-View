#include <math.h>
#include <string.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"
// Variaveis globais a serem utilizadas:
// Dimensoes da imagem de entrada
int sizeX, sizeY;
int imageSize;

// Imagem de entrada
unsigned char* image;
// Imagem de saída
unsigned char* image8;
// Fator de exposição inicial
float exposure = 1.0;
//Fator de conversao
float c = 0;
//Constante limitadora
float k = 0.3f;


///////////////////////////////////////////////////////////
//Funcao auxiliar para converter inteiro para uma string//
///////////////////////////////////////////////////////////
/*char* intToStr(int x){
    int i,len;
    len = 0;
    char *str;
    for(i = x;i > 0;i /= 10)
        len++;
    str = (char *)malloc(len+1);
    for(i = len-1;i >= 0;i--){
        str[i] = (char)((x%10)+48);
        x /= 10;
    }
    str[len] = '\0';
    return str;
}*/

/////////////////////////////////
//Escreve header PPM no arquivo//
/////////////////////////////////
void writePPMHeader(FILE *f){
    fprintf(f,"P6\n%d %d\n255\n",sizeX,sizeY);
}

////////////////////////////////////;////////////////////////////////////////
//Salva a imagem no estado atual em formato PPM com componentes de cor 8bit//
/////////////////////////////////////////////////////////////////////////////
void createPPM(char *fName){
    int i;
    unsigned char *ppmIMG = (unsigned char*)malloc(imageSize);
    FILE *f = fopen(fName, "wb");
    if(f == NULL){
        printf("Erro na criacao do arquivo!\n");
        exit(1);
    }
    writePPMHeader(f);
    for(i = 0;i < imageSize;i++){
        ppmIMG[i] = image8[i];
    }
    int nBytes = fwrite(ppmIMG,sizeof(unsigned char), imageSize, f);
    if(nBytes != imageSize){
        printf("Erro na gravacao da imagem!\n");
        exit(1);
    }
    fclose(f);
    printf("Out.ppm criada!\n");
    free(ppmIMG);
}

///////////////////////////////////////
//Exibe teclas de comando disponiveis//
///////////////////////////////////////
void instructions(){
    printf("\nComandos:\nCima/Baixo - Altera constante de Tone Mapping\n");
    printf("Esquerda/Direita - Altera fator de exposicao\n");
    printf("Scroll - Zoom\n");
    printf("F1 - Salvar imagem em formato PPM (8-bit)\n");
    printf("ESC - Sair\n\n");
}

/////////////////////////////////////////////////////////////
//Checa id no header para ver se trata-se de um arquivo HDF//
/////////////////////////////////////////////////////////////
void checkHDF(unsigned char *img){
    if(strncmp((char *)img,"HDF",3) != 0){
        printf("Imagem invalida!\n");
        exit(1);
    }
    printf("Imagem OK!\n");
}

/////////////////////////////////////////////////////////////
//Aplica tone mapping em um componente de cor do tipo float//
/////////////////////////////////////////////////////////////
float applyTM(float f,float k){
    return f/(f+k);
}

///////////////////////////////////////////////////////////////
//Converte componente de cor do tipo float para unsigned char//
///////////////////////////////////////////////////////////////
unsigned char to8bit(float comp){
    return (unsigned char)((int)(comp*255.0f));
}

///////////////////////////////////////////////////////////////////
//Aplica fator de exposicao em um componente de cor do tipo float//
///////////////////////////////////////////////////////////////////
float applyExp(float expo, float value){
    return expo*value;
}

///////////////////////////////////////////////////////////
//Converte byte para float a partir do fator de conversao//
///////////////////////////////////////////////////////////
float convert(unsigned char b, float fac){
    return fac*(float)b;
}

// Funcao principal de processamento
void process()
{
    printf("Exposure: %.3f\nConstant: %.3f\n", exposure, k);
    unsigned char* ptr = image;
    ptr += 11;//Pula header
    int i = 0;
    int pos;
    for(pos = 3;pos < sizeX*sizeY*4;pos += 4){//comeca na mantissa
            c = pow(2,ptr[pos]-136);
            image8[i++] = to8bit(applyTM(applyExp(convert(ptr[pos-3],c),exposure),k));
            image8[i++] = to8bit(applyTM(applyExp(convert(ptr[pos-2],c),exposure),k));
            image8[i++] = to8bit(applyTM(applyExp(convert(ptr[pos-1],c),exposure),k));
    }
    buildTex();
}

///////////////////////////
//Pega tamanho do arquivo//
///////////////////////////
unsigned long getSize(FILE* f)
{
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fseek(f, SEEK_SET, 0);
    return len;
}

////////////////////////////
//Pega dimensoes da imagem//
////////////////////////////
void getDim(unsigned char *img ,int *width,int *height){
    img += 3;//Pula o id
    *width = (*((int *)img));
    img += 4;//Pula largura
    *height = (*((int *)img));
    printf("Dimensoes:%d x %d\n",*width,*height);
}

//////////////////////////////////////
//Le imagem e carrega ela na memoria//
//////////////////////////////////////
unsigned char* readImg(char *fName){
    printf("Lendo imagem...\n");
    FILE *f = fopen(fName,"rb");
    if(f == NULL) {
        printf("Arquivo %s nao existe.\n", fName);
        exit(1);
    }
    unsigned long size = getSize(f);
    unsigned char *img = (unsigned char*)malloc(sizeof(unsigned char)*size);
    unsigned long readSize;
    readSize = fread(img, sizeof(unsigned char),size,f);
    if(readSize != size){
        printf("Erro na leitura do arquivo!\n");
        exit(1);
    }
    printf("Checando imagem...\n");
    checkHDF(img);
    fclose(f);
    return img;
}

int main(int argc, char** argv)
{
    if(argc==1) {
        printf("%s [image file.hdf]\n",argv[0]);
        exit(1);
    }
    // Inicializacao da janela grafica
    init(argc,argv);
    sizeX = 0;
    sizeY = 0;
    image = readImg(argv[1]);//Le imagem de entrada
    getDim(image,&sizeX,&sizeY);//Pega dimensao da imagem
    imageSize = sizeX*sizeY*3;
    // Aloca memoria para imagem de saida (RGB de 24 bits)
    image8 = (unsigned char*)malloc((sizeof(unsigned char)*imageSize));
    exposure = 1.0f; // exposicao inicial
    // Aplica processamento inicial
    process();
    instructions();
    glutMainLoop();
    return 0;
}
