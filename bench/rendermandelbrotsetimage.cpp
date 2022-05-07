#include <iostream>
#include <fstream>
#include <math.h>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <Taskus.h>


#define MAX_ITERATIONS 1000


/* On this benchmark there is a limitation when running in multithreaded mode, the right side of the image
    takes more iterations to render each pixel which means that the threads that are running on the left
    of the image take way less time and become idle (which ruins the total cpu time by half)).
    There's currently no way we can fix this without some huge overhead

*/

// __attribute__((__packed__)) doesnt allow the compiler to align the struct

#pragma pack(push,1)
struct Pixel{
    uint8_t r;
    uint8_t g;
    uint8_t b;    
};
#pragma pack(pop)

#pragma pack(push,1)
struct BitMapHeader{
    char header[2] = {'B', 'M'};
    uint32_t BMPSize;
    uint16_t reserverd0 = 0;
    uint16_t reserverd1 = 0;
    uint32_t offSetBMPData;


};
#pragma pack(pop)

#pragma pack(push,1)

struct BitmapInfoHeader{
    uint32_t size = 40;
    int width;
    int height;
    uint16_t colorplanes = 1;
    uint16_t bitsPerPixel = 24; //8 bits for each
    uint32_t compression = 0; //disable compression
    uint32_t imageSize = 0; //only used when compression is enabled
    int horizonalResolution = 2755; //70 dpi
    int verticalResolution = 2755; //70 dpi
    uint32_t numberColors = 0;
    uint32_t importantColors = 0; //every color is important 
};
#pragma pack(pop)

typedef std::vector<std::vector<Pixel>> ImageVec;

void writeToImage(std::string filename, ImageVec pixel_list){
    BitMapHeader header;
    BitmapInfoHeader DIBHeader;
    std::vector<char> BMPImageData;
    if(pixel_list.size() == 0) throw new std::runtime_error("Pixel List has no rows!");
    DIBHeader.height = pixel_list.size();
    DIBHeader.width = pixel_list[0].size();

    header.offSetBMPData = sizeof(BitmapInfoHeader) + sizeof(BitMapHeader);
    
    //insert data to bmpimage data
    for(auto itrow = pixel_list.rbegin(); itrow != pixel_list.rend(); itrow++){
        for(Pixel p : *itrow){
            //this seems like unnecessary complexity for the code but it makes it more dynamic if I want to change the 
            //pixels per image format
            char * cp = (char *) ((void*) &p);
            for(int i = 0; i < sizeof(Pixel); i++){
                BMPImageData.push_back(cp[i]);
            }
        }
        //Every row must be a multiple of 4
        int neededoffset = BMPImageData.size() % 4;
        if(neededoffset != 0) neededoffset = 4 - neededoffset;
        for(int i = 0; i < neededoffset; i++){
            BMPImageData.push_back(0);
        }
    }
    header.BMPSize = header.offSetBMPData + BMPImageData.size();
    DIBHeader.imageSize = BMPImageData.size();
    

    std::cout << "Writing to file...";
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<char*>(&header), sizeof(BitMapHeader));
    file.write(reinterpret_cast<char*>(&DIBHeader), sizeof(BitmapInfoHeader));
    file.write(BMPImageData.data(), BMPImageData.size());
    file.close();

}

struct Coords{
    int x = -1;
    int y = -1;
};



bool getNextCoord(const Coords& start,const Coords& end, Coords& c){
    if(c.x == -1){
        c = start;
        return true;
    }
    c.x++;
    if(c.x > end.x && c.y == end.y){
        return false;
    }
    if(c.x > end.x){
        c.x = start.x;
        c.y++;
        return true;
    }
    return true;
}

ImageVec drawPartOfImage(Coords start, Coords end, int w, int h){
    //allocate memory for the part of this image
    ImageVec partImage;
    partImage.resize(end.y - start.y + 1);
    for(auto it = partImage.begin(); it != partImage.end(); it++){
        (*it).resize(end.x - start.x + 1);
    }
    //on the x axis it is between -2.00 and 0.47
    //on the y axis it is between -1.12, 1.12
    Coords pixel;
    while(getNextCoord(start, end, pixel)){
        //mandelbrot calculation
        double x0 = ((0.47 - (-2.00)) / ((double) w)) *pixel.x - 2.00;
        double y0 = ((1.12 - (-1.12)) / ((double) h)) *pixel.y - 1.12;
        double x = 0;
        double y = 0;
        int iteration = 0;
        while(x*x + y*y <= 2*2 && iteration < MAX_ITERATIONS){
            double xtemp = x*x - y*y + x0;
            y = 2*x*y + y0;
            x = xtemp;
            iteration++;
        }
        //took from stackoverflow
            int i = iteration % 16;
            Pixel mapping[16];
            mapping[0] = {66, 30, 15};
            mapping[1] = {25, 7, 26};
            mapping[2] = {9, 1, 47};
            mapping[3] = {4, 4, 73};
            mapping[4] = {0, 7, 100};
            mapping[5] = {12, 44, 138};
            mapping[6] = {24, 82, 177};
            mapping[7] = {57, 125, 209};
            mapping[8] = {134, 181, 229};
            mapping[9] = {211, 236, 248};
            mapping[10] = {241, 233, 191};
            mapping[11] = {248, 201, 95};
            mapping[12] = {255, 170, 0};
            mapping[13] = {204, 128, 0};
            mapping[14] = {153, 87, 0};
            mapping[15] = {106, 52, 3};

        partImage[pixel.y-start.y][pixel.x - start.x] = mapping[i];
    }
    return partImage;
}


int width = 4098;
int height = 4098;

class startTask : public Taskus::Task{
    public:
        void tryMutate(){

        }

        void runTaskFunction(){

        }
};

class imagePartCalcTask : public Taskus::Task{
    public:
        imagePartCalcTask(Coords nstart, Coords nend, ImageVec* npartImage) : Task(){
            start = nstart;
            end = nend;
            partImage = npartImage;
        }
        

        void tryMutate(){
            
        }

        void runTaskFunction(){
            *partImage = drawPartOfImage(start, end, width, height);
        }
    private:
        Coords start;
        Coords end;
        ImageVec* partImage;
};

struct DividedImage{
    Coords start;
    Coords end;
    ImageVec* image = nullptr;
};

ImageVec* createImage(Coords start, Coords end){
    ImageVec* r = new ImageVec();
    r->resize(end.y - start.y + 1);
    for(auto it = r->begin(); it != r->end(); it++){
        (*it).resize(end.x - start.x + 1);
    }
    return r;
}

class writeImageToFile : public Taskus::Task{
    public:
        writeImageToFile(std::vector<DividedImage>* nparts) : Task(){
            parts = nparts;
            finalImage = createImage({0,0}, {width,height});
        }

        ~writeImageToFile(){
            for(int i = 0; i < parts->size(); i++){
                ImageVec * t = parts->at(i).image;
                t->clear();
                delete t;
            }
            delete finalImage;
            parts->clear();
        }

        void assembleImage(){

            for(auto it = parts->begin(); it != parts->end(); it++){
                Coords t = {-1, -1};
                getNextCoord((*it).start, (*it).end, t);
                while(getNextCoord((*it).start, (*it).end, t)){
                    (*finalImage)[t.y][t.x] = (*(*it).image)[t.y - (*it).start.y][t.x - (*it).start.x];
                }                
            }
        }

        void tryMutate(){}

        void runTaskFunction(){
            //this assumes of course that the calculation has indeed stopped, it is guarenteed by taskus, in theory
            assembleImage();
            writeToImage("mandelbrotelsetimage.bmp", *finalImage);

            std::cout << "Done\n";
        }


    private:
        std::vector<DividedImage>* parts;

        ImageVec * finalImage;
};




std::vector<DividedImage> divideImage(Coords start, Coords end,int n_times){
    if(n_times <= 0) return {};
    int w = end.x - start.x;
    int h = end.y - start.y;
    DividedImage d1;
    DividedImage d2;
    if(h > w){
        std::cout << "Horizontal\n";
        d1.start = start;
        d1.end = {end.x, start.y + (end.y - start.y)/2};
        d2.start = {start.x, start.y + (end.y - start.y)/2 + 1};
        d2.end = end;
        d1.image = createImage(d1.start, d1.end);
        d2.image = createImage(d2.start, d2.end);
    } else {
        std::cout << "Vertical\n";
        d1.start = start;
        d1.end ={start.x + (end.x - start.x)/2, end.y};
        d2.start ={start.x + (end.x - start.x)/2 + 1, start.y};
        d2.end = end;
        d1.image = createImage(d1.start, d1.end);
        d2.image = createImage(d2.start, d2.end);
        
    }
    n_times--;
    std::vector<DividedImage> r1 = divideImage(d1.start, d1.end, n_times);
    std::vector<DividedImage> r2 = divideImage(d2.start, d2.end, n_times);
    for(int i = 0; i < r2.size(); i++){
        r1.push_back(r2[i]);
    }
    if(r1.empty()) return {d1, d2};
    return r1;
    
}



int main(int argc, char ** argv){
    if(argc <= 1) {
        std::cout << "This program requires at least a --multi or a --single option" << "\n";
        return 1;
    }

    std::string t = argv[1];
    if(t == "--multi"){
        Taskus::TaskPool tPool;
        tPool.start();
        startTask s;

        writeImageToFile * wt = nullptr;
        std::vector<imagePartCalcTask*> tasks;

        int t = 1;
        int i = 0;

        /*
            we devide the image by 4 times the amount of threads to parellize this to avoid
            a single cpu core running on a slow big chunk of the image. We also shuffle them around
            to avoid this kind of behaviour.
            This should be done automatically by Taskus when using the new FunctionTask, when possible 
            because on this situation for example is not straightforward at least because how images are divided
        */
        for(; t < tPool.getMaxNumOfThreads()*4; i++){
            t *= 2;
        }

        std::vector<DividedImage> dimages = divideImage({0,0}, {width, height}, i);

        std::shuffle(dimages.begin(), dimages.end(), std::random_device());

        for(DividedImage image : dimages){
            imagePartCalcTask * c = new imagePartCalcTask(image.start, image.end, image.image);
            tasks.push_back(c);
        }

        //this assumes that they are in order 
        wt = new writeImageToFile(&dimages);

        for(auto it = tasks.begin(); it != tasks.end(); it++){
            s += (*it);
            *(*it) += wt;
        }

        tPool.addTask(&s);

        tPool.stop();

        delete wt;

        for(imagePartCalcTask * t: tasks){
            delete t;
        }

        return 0;
    } else if(t == "--single"){
        ImageVec c;
        
        writeToImage("mandelbrotelsetimage.bmp", drawPartOfImage({0,0}, {1023, 1023}, 1024, 1024));
        return 0;
    }
    std::cout << "Didn't provide a valid option" << "\n";
}