#include <color/color.h>

void ImageConverter::imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cvImage;
    try
    {
      cvImage = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
    // shelf is used for testing in gazebo
    int shelfLowerB = 30;
    int shelfLowerG = 80;
    int shelfLowerR = 160;
    int shelfUpperB = 80;
    int shelfUpperG = 150;
    int shelfUpperR = 220; 


    
    int blueLowerB = 50;
    int blueLowerG = 0;
    int blueLowerR = 0;
    int blueUpperB = 110;
    int blueUpperG = 20;
    int blueUpperR = 25; 
    
    int greenLowerB = 0;
    int greenLowerG = 30;
    int greenLowerR = 0;
    int greenUpperB = 45;
    int greenUpperG = 80;
    int greenUpperR = 25; 

    int redLowerB = 0;
    int redLowerG = 0;
    int redLowerR = 100;
    int redUpperB = 25 ;
    int redUpperG = 10;
    int redUpperR = 160; 
    



    
    cv::Mat mask, part, maskBlue, maskGreen, maskRed, maskShelf;
    

    cv::inRange(cvImage->image, cv::Scalar(blueLowerB,blueLowerG,blueLowerR), cv::Scalar(blueUpperB,blueUpperG,blueUpperR), maskBlue);
    cv::inRange(cvImage->image, cv::Scalar(greenLowerB,greenLowerG,greenLowerR), cv::Scalar(greenUpperB,greenUpperG,greenUpperR), maskGreen);
    cv::inRange(cvImage->image, cv::Scalar(redLowerB,redLowerG,redLowerR), cv::Scalar(redUpperB,redUpperG,redUpperR), maskRed);

    mask = maskBlue + maskGreen + maskRed;
  
    cv::inRange(cvImage->image, cv::Scalar(shelfLowerB,shelfLowerG,shelfLowerR), cv::Scalar(shelfUpperB,shelfUpperG,shelfUpperR), maskShelf);
    part = mask(cv::Range(400,480),cv::Range(200,465));

    cv::namedWindow(OPENCV_WINDOW,600);
    cv::imshow(OPENCV_WINDOW, part);
    cv::imshow(OPENCV_WINDOW_UNCROPPED, mask);


    cv::Mat blob = ~maskShelf;
    
    using namespace cv;
    using namespace std;


	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;


    params.thresholdStep = 10;
    params.minThreshold = 50;
    params.maxThreshold = 220;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 25;
  params.maxArea = 100000;

  // filter by color
  params.filterByColor = true;
  params.blobColor = 0;
	
  // Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.4;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.71;


  Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params); 
  
  // Storage for blobs
	vector<KeyPoint> keypoints; 

 detector->detect( blob, keypoints);
  

	Mat im_with_keypoints;
	drawKeypoints( blob, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  
 // detector->detect( blob, keypoints);
  cout<<keypoints[1].pt<<endl;
  imshow("keypoints", im_with_keypoints);

  
    int whitePixels_blue = cv::countNonZero(maskBlue);
    int whitePixels_red = cv::countNonZero(maskRed);
    int whitePixels_green = cv::countNonZero(maskGreen);
    int whitePixels = cv::countNonZero(mask);
    //std::cout << whitePixels << std::endl;
    
    if (whitePixels_blue >= 3000 && trashDetected_blue == false)
    {
      std::cout << "trash detected (blue)" << std::endl;
      trashDetected_blue = true;
    }
    if (whitePixels_red >= 3000 && trashDetected_red == false)
    {
      std::cout << "trash detected (red)" << std::endl;
      trashDetected_red = true;
    }
    if (whitePixels_green >= 3000 && trashDetected_green ==false)
    {
      std::cout << "trash detected (green)" << std::endl;
      trashDetected_green = true;
    }
    
    if (whitePixels < 200)
    {
      trashDetected_blue = false;
      trashDetected_green = false;
      trashDetected_red = false;
    }
    

    cv::waitKey(3);

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW_ORIGINAL, cvImage->image);
    cv::waitKey(3);

    // Output modified video stream
    //image_pub_.publish(cvImage->toImageMsg());

    
  }
/*int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}*/