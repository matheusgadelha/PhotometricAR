#ifndef ARPIPELINE_HPP
#define ARPIPELINE_HPP

////////////////////////////////////////////////////////////////////
// File includes:
#include "PatternDetector.hpp"
#include "CameraCalibration.hpp"
#include "GeometryTypes.hpp"

#include <algorithm>
#include <cmath>

struct LightFactor
{
	cv::Scalar color;
	float mean;

	LightFactor( cv::Scalar _color, float _mean)
	{
		this->color = _color;
		this->mean = _mean;
	}

	bool operator < ( const LightFactor l) const
	{
		return this->mean < l.mean;
	}
};

class ARPipeline {
public:
    ARPipeline( const cv::Mat& mainPattern, const cv::Mat& frontPattern,
            const cv::Mat& rightPattern, const cv::Mat& leftPattern,
            const CameraCalibration& calibration);
    ARPipeline(const cv::Mat& patternImage, const CameraCalibration& calibration);

    bool processFrame(cv::Mat& inputFrame);

    const Transformation& getPatternLocation() const;

    PatternDetector m_patternDetector;
    
    cv::Point3f computeLightDirection();
    cv::Scalar computeLightColor( cv::Mat& org, cv::Mat& rec );
    cv::Scalar computeLight( cv::Mat& org, cv::Mat& rec );

	void divergenceMask( cv::Mat& org, cv::Mat& rec, cv::Mat& result );
    
    cv::Scalar currentDiffuseColor;
    cv::Scalar currentAmbientLight;

private:
    CameraCalibration m_calibration;
    Pattern m_mainPattern;
    Pattern m_frontPattern;
    Pattern m_rightPattern;
    Pattern m_leftPattern;
    PatternTrackingInfo m_patternInfo;

	cv::Scalar mainLightFactor;
	cv::Scalar frontLightFactor;
	cv::Scalar rightLightFactor;
	cv::Scalar leftLightFactor;
    
    bool left( cv::Point2f, cv::Point2f, cv::Point2f );
    
    bool isLeftVisible;
    bool isRightVisible;
    bool isFrontVisible;
    
    cv::Mat sFront, sRight, sLeft, sMain;
    
    float small_rate;
    
    cv::Mat m_frontHomography, m_rightHomography, m_leftHomography, m_mainHomography;
    cv::Mat m_rectifiedFront, m_rectifiedRight, m_rectifiedLeft, m_rectifiedMain;
    
    //PatternDetector     m_patternDetector;
};

#endif
