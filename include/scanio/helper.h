#ifndef __SCAN_IO_HELPER_H__
#define __SCAN_IO_HELPER_H__

#include <boost/filesystem/operations.hpp>
#include <iostream>
#include "slam6d/pointfilter.h"
#include "slam6d/io_types.h"

class ScanDataTransform {
    public:
        virtual bool transform(double xyz[3], unsigned char rgb[3], float*  refl, float* temp, float* ampl, int* type, float* devi) = 0;
};

class ScanDataTransform_identity : public ScanDataTransform {
    public:
        bool transform(double xyz[3], unsigned char rgb[3], float*  refl, float* temp, float* ampl, int* type, float* devi);
};

class ScanDataTransform_ks : public ScanDataTransform {
    public:
        bool transform(double xyz[3], unsigned char rgb[3], float*  refl, float* temp, float* ampl, int* type, float* devi);
};

class ScanDataTransform_riegl : public ScanDataTransform {
    public:
        bool transform(double xyz[3], unsigned char rgb[3], float*  refl, float* temp, float* ampl, int* type, float* devi);
};

class ScanDataTransform_rts : public ScanDataTransform {
    public:
        bool transform(double xyz[3], unsigned char rgb[3], float*  refl, float* temp, float* ampl, int* type, float* devi);
};

class ScanDataTransform_xyz : public ScanDataTransform {
    public:
        bool transform(double xyz[3], unsigned char rgb[3], float*  refl, float* temp, float* ampl, int* type, float* devi);
};

std::list<std::string> readDirectoryHelper(
        const char* dir_path,
        unsigned int start,
        unsigned int end,
        const char** data_path_suffix,
        const char* data_path_prefix = "scan",
        unsigned int id_len = 3);
void readPoseHelper(
        const char* dir_path,
        const char* identifier,
        double* pose,
        const char* pose_path_suffix = ".pose",
        const char* pose_path_prefix = "scan");
std::streamsize uosHeaderTest(std::istream& infile, char **line, std::streamsize bufsize = 128);
bool uosHeaderTest(char *line);
std::function<bool (std::istream &data_file)> open_uos_file(
        IODataType* spec, ScanDataTransform& transform, PointFilter& filter,
        std::vector<double>* xyz, std::vector<unsigned char>* rgb,
        std::vector<float>* reflectance, std::vector<float>* temperature,
        std::vector<float>* amplitude, std::vector<int>* type,
        std::vector<float>* deviation);
bool readASCII(std::istream& infile,
        char *firstline,
        std::streamsize lenfirstline,
        IODataType* spec,
        ScanDataTransform& transform,
        PointFilter& filter,
        std::vector<double>* xyz = 0,
        std::vector<unsigned char>* rgb = 0,
        std::vector<float>* reflectance = 0,
        std::vector<float>* temperature = 0,
        std::vector<float>* amplitude = 0,
        std::vector<int>* type = 0,
        std::vector<float>* deviation = 0,
        std::streamsize bufsize = 128);

bool open_path(boost::filesystem::path data_path, std::function<bool (std::istream &)>);
bool open_path_writing(boost::filesystem::path data_path, std::function<bool (std::ostream &)> handler);
bool write_multiple(std::map<std::string,std::string> contentmap);

#endif

/* vim: set ts=4 sw=4 et: */
