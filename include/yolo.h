// Created by luozhiwang (luozw1994@outlook.com)
// Date: 2020/3/14

#ifndef TENSORRT_YOLO_H
#define TENSORRT_YOLO_H

#include <thread>
#include <mutex>

#include "tensorrt.h"
#include "utils.h"

class Yolo : private TensorRT{
private:
    common::DetectParams mYoloParams;
    std::mutex mMutex;
private:

    //! private func for postProcess
    //! \param start
    //! \param length
    //! \param postThres
    //! \param origin_output
    //! \param bboxes
    void postProcessParall(unsigned long start, unsigned long length, float postThres, const float *origin_output, std::vector<common::Bbox> *bboxes);

    //! Ensure thread safety
    //! \param bboxes
    //! \param bbox
    void safePushBack(std::vector<common::Bbox> *bboxes, common::Bbox *bbox);

    //! If the image is padded, bboxes need to be restored.
    //! \param ih Input image height
    //! \param iw Input image width
    //! \param oh Output image height
    //! \param ow Output image width
    //! \param bboxes common::Bbox{xmin, ymin, xmax, ymax, cid, prob}
    void transformBbx(const int &ih, const int &iw, const int &oh, const int &ow, std::vector<common::Bbox> &bboxes, bool is_padding=true);

public:

    //! Initializing
    //! \param inputParams
    //! \param trtParams
    //! \param yoloParams
    Yolo(common::InputParams inputParams, common::TrtParams trtParams, common::DetectParams yoloParams);

    //! Read images into buffer
    //! \param images
    //! \return Float32 file data
    std::vector<float> preProcess(const std::vector<cv::Mat> &images) const;

    //! Post Process for Yolov3
    //! \param bufferManager It contains inference result
    //! \param postThres
    //! \param nms
    //! \return [xmin, ymin, xmax, ymax]
    std::vector<common::Bbox> postProcess(common::BufferManager &bufferManager, float postThres=-1, float nmsThres=-1);

    //! Init Inference Session
    //! \param initOrder 0==========> init from SerializedPath. If failed, init from onnxPath.
    //!                             1 ==========> init from onnxPath and save the session into SerializedPath if it doesnt exist.
    //!                             2 ==========> init from onnxPath and force to save the session into SerializedPath.
    //! \return true if no errors happened.
    bool initSession(int initOrder) override;

    //!
    //! \param image
    //! \param posThres Post process threshold.
    //! \param nmsThres NMS Threshold
    //! \return [xmin, ymin, xmax, ymax]
    std::vector<common::Bbox> predOneImage(const cv::Mat &image, float postThres=-1, float nmsThres=-1);

};

#endif //TENSORRT_YOLO_H
