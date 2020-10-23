package uk.ac.ic.benchmatte.util

import matting.OpenCVMatter
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.imgcodecs.Imgcodecs
import uk.ac.ic.matterialize.matting.BackgroundNegationMatter
import uk.ac.ic.matterialize.matting.FaceDetectionMatter
import uk.ac.ic.matterialize.matting.KMeansMatter
import uk.ac.ic.matterialize.matting.Matter
import uk.ac.ic.matterialize.matting.MatterMode

class Benchmark(private val maskPath: String, private val backgroundPath: String, private val foregroundPath: String) {
    lateinit var background: Mat
    lateinit var foreground: Mat

    lateinit var composed: Mat
    lateinit var mask: Mat

    private val matters = mutableListOf<Pair<MatterMode, Matter>>()

    fun setup() {
        background = Compositor.loadFile(backgroundPath)
        foreground = Compositor.loadFile(foregroundPath)

        val pair = Compositor.compose(background, foreground, Compositor.loadFile(maskPath, CvType.CV_8U))
        composed = pair.first
        mask = pair.second

        MatterMode.values().forEach {
            val matter = when (it) {
                MatterMode.KMeans -> KMeansMatter(background)
                MatterMode.FaceDetection -> FaceDetectionMatter()
                MatterMode.BackgroundNegation -> BackgroundNegationMatter(background)
                MatterMode.OpenCV -> OpenCVMatter()
            }

            // the initial run is discarded to account for initialisation times
            matter.foregroundMask(composed)

            matters.add(Pair(it, matter))
        }
    }

    fun export() {
        Imgcodecs.imwrite("_composed.png", composed)
        Imgcodecs.imwrite("_mask.png", mask)
    }

    fun run(): List<Triple<MatterMode, Double, Long>> {
        return matters.map { (mode, matter) ->
            println(mode)

            val start = System.currentTimeMillis()
            val result = matter.foregroundMask(composed)
            val end = System.currentTimeMillis()

            Imgcodecs.imwrite("_$mode.png", result)

            val score = Scorer.percentage(result, mask)

            Triple(mode, score, end - start)
        }
    }
}
