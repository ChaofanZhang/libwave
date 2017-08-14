#include <chrono>

#include "wave/wave_test.hpp"
#include "wave/vision/utils.hpp"
#include "wave/vision/detector/fast_detector.hpp"
#include "wave/vision/detector/orb_detector.hpp"
#include "wave/vision/descriptor/brisk_descriptor.hpp"
#include "wave/vision/descriptor/orb_descriptor.hpp"
#include "wave/vision/matcher/brute_force_matcher.hpp"
#include "wave/vision/tracker/tracker.hpp"

namespace wave {

const auto TEST_IMAGE_0 = "tests/data/tracker_test_sequence/frame0059.jpg";
const auto TEST_IMAGE_1 = "tests/data/tracker_test_sequence/frame0060.jpg";
const auto TEST_IMAGE_2 = "tests/data/tracker_test_sequence/frame0061.jpg";

TEST(TrackerTests, ConstructorTest) {
    FASTDetector detector1;
    BRISKDescriptor descriptor1;
    ORBDetector detector2;
    ORBDescriptor descriptor2;
    BruteForceMatcher matcher;

    // Cannot define ASSERTS here, as gtest is very sensitive to commas.
    Tracker<FASTDetector, BRISKDescriptor, BruteForceMatcher> tracker1(
      detector1, descriptor1, matcher);

    Tracker<ORBDetector, ORBDescriptor, BruteForceMatcher> tracker2(
      detector2, descriptor2, matcher);

    SUCCEED();
}

TEST(TrackerTests, AddImageGetTracks) {
    // Define detectors, descriptors, and matcher.
    FASTDetector detector;
    BRISKDescriptor descriptor;
    BruteForceMatcher matcher;

    std::chrono::steady_clock clock;

    // Define 2 trackers, but tracker 2 is in online mode.
    Tracker<FASTDetector, BRISKDescriptor, BruteForceMatcher> tracker(
      detector, descriptor, matcher);

    // Define tracker 2 in online mode.
    Tracker<FASTDetector, BRISKDescriptor, BruteForceMatcher> tracker2(
      detector, descriptor, matcher);

    cv::Mat image_1 = cv::imread(TEST_IMAGE_0);
    cv::Mat image_2 = cv::imread(TEST_IMAGE_1);
    cv::Mat image_3 = cv::imread(TEST_IMAGE_2);

    // Add image 1 to both trackers.
    auto time1 = clock.now();
    tracker.addImage(image_1, time1);
    tracker2.addImage(image_1, time1);

    // Extracting tracks for images that don't exist should throw exceptions.
    ASSERT_THROW(tracker.getTracks(-1), std::out_of_range);
    ASSERT_THROW(tracker.getTracks(5), std::out_of_range);
    ASSERT_THROW(tracker2.getTracks(-1), std::out_of_range);
    ASSERT_THROW(tracker2.getTracks(5), std::out_of_range);

    // The tracks from the first image should be empty (no sequence for tracks)
    std::vector<FeatureTrack> ft0_t1 = tracker.getTracks(0);
    ASSERT_TRUE(ft0_t1.empty());

    std::vector<FeatureTrack> ft0_t2 = tracker2.getTracks(0);
    ASSERT_TRUE(ft0_t2.empty());

    // Add image 2 to both trackers.
    auto time2 = clock.now();
    tracker.addImage(image_2, time2);
    tracker2.addImage(image_2, time2);

    // Add image 3 to both trackers.
    auto time3 = clock.now();
    tracker.addImage(image_3, time3);
    tracker2.addImage(image_3, time3);

    // Get tracks from both images, make sure in both cases they are not false.
    std::vector<FeatureTrack> ft1_t1 = tracker.getTracks(1);
    std::vector<FeatureTrack> ft2_t1 = tracker.getTracks(2);
    std::vector<FeatureTrack> ft1_t2 = tracker2.getTracks(1);
    std::vector<FeatureTrack> ft2_t2 = tracker2.getTracks(2);

    ASSERT_FALSE(ft1_t1.empty());
    ASSERT_FALSE(ft2_t1.empty());
    ASSERT_FALSE(ft1_t2.empty());
    ASSERT_FALSE(ft2_t2.empty());
}

TEST(TrackerTests, OfflineTrackerNoImages) {
    std::vector<cv::Mat> image_sequence;
    FASTDetector detector;
    BRISKDescriptor descriptor;
    BruteForceMatcher matcher;

    Tracker<FASTDetector, BRISKDescriptor, BruteForceMatcher> tracker(
      detector, descriptor, matcher);

    ASSERT_THROW(tracker.offlineTracker(image_sequence), std::invalid_argument);
}

TEST(TrackerTests, OnlineTrackerTest) {
    ORBDetector detector;
    ORBDescriptor descriptor;
    BruteForceMatcher matcher;

    SUCCEED();
}

}  // namespace wave
