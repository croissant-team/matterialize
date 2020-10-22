package uk.ac.ic.benchmatte

import matting.OpenCVMatter
import org.opencv.core.CvType
import org.opencv.core.Mat
import uk.ac.ic.benchmatte.util.Compositor
import uk.ac.ic.benchmatte.util.Scorer
import uk.ac.ic.matterialize.matting.BackgroundNegationMatter
import uk.ac.ic.matterialize.matting.FaceDetectionMatter
import uk.ac.ic.matterialize.matting.KMeansMatter
import uk.ac.ic.matterialize.matting.MatterMode

fun main() {
    nu.pattern.OpenCV.loadLocally()

    val targetMask = Compositor.loadFile("", CvType.CV_8U)
    val background = Compositor.loadFile("")
    val foreground = Compositor.loadFile("")

    val (composed, newMask) = Compositor.compose(background, foreground, targetMask)

    println(runMatter(MatterMode.BackgroundNegation, background, composed, newMask))
    println(runMatter(MatterMode.FaceDetection, null, composed, newMask))
}

fun runMatter(mode: MatterMode, clean: Mat?, composed: Mat, expected: Mat): Pair<Double, Long> {
    val matter = when (mode) {
        MatterMode.KMeans -> KMeansMatter(clean!!)
        MatterMode.FaceDetection -> FaceDetectionMatter()
        MatterMode.BackgroundNegation -> BackgroundNegationMatter(clean!!)
        MatterMode.OpenCV -> OpenCVMatter()
    }

    // the initial run is discarded to account for initialisation times
    matter.foregroundMask(composed)

    val start = System.currentTimeMillis()
    val result = matter.foregroundMask(composed)
    val end = System.currentTimeMillis()

    val score = Scorer.percentage(result, expected)

    return Pair(score, end - start)
}