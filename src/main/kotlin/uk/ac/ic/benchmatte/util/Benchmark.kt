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
import java.io.File
import uk.ac.ic.matterialize.matting.backgroundcut.BackgroundCutMatter

class Benchmark(private val masksPath: String, private val backgroundsPath: String, private val foregroundsPath: String) {
    val backgrounds = mutableListOf<Mat>()
    val foregrounds = mutableListOf<Mat>()

    val composeds = mutableListOf<Mat>()
    val masks = mutableListOf<Mat>()

    private val matters = mutableListOf<Triple<MutableList<Pair<MatterMode, Matter>>, Mat, Mat>>()

    fun setup() {

        File(backgroundsPath).listFiles().zip(File(foregroundsPath).listFiles()).forEach { (bg, fg) ->

            println("Composing (bg) $bg with (fg) $fg")

            val bgMat = Compositor.loadFile(bg.path)
            val fgMat = Compositor.loadFile(fg.path)

            backgrounds.add(bgMat)
            foregrounds.add(fgMat)

            val pair = Compositor.compose(bgMat, fgMat, Compositor.loadFile(masksPath + "/" + fg.name, CvType.CV_8U))
            composeds.add(pair.first)
            masks.add(pair.second)

            val imgMatters = mutableListOf<Pair<MatterMode, Matter>>()

            MatterMode.values().forEach {
                val matter = when (it) {
                    MatterMode.KMeans -> KMeansMatter(bgMat)
                    MatterMode.FaceDetection -> FaceDetectionMatter()
                    MatterMode.BackgroundNegation -> BackgroundNegationMatter(bgMat)
                    MatterMode.OpenCV -> OpenCVMatter()
                    MatterMode.BackgroundCut -> BackgroundCutMatter(bgMat)
                }

                // the initial run is discarded to account for initialisation times
                matter.backgroundMask(pair.first)

                imgMatters.add(Pair(it, matter))
            }

            matters.add(Triple(imgMatters, pair.first, pair.second))
        }
    }

    fun export() {
        (0 until composeds.size).forEach { i ->
            Imgcodecs.imwrite("images/output/${i}_composed.png", composeds[i])
            Imgcodecs.imwrite("images/output/${i}_mask.png", masks[i])
        }
    }

    fun run(): List<List<Triple<MatterMode, Array<IntArray>, Long>>> {
        var i = 0
        return matters.map { (imgMatter, composed, mask) ->
            i++
            imgMatter.map { (mode, matter) ->
                val start = System.currentTimeMillis()
                val result = matter.backgroundMask(composed)
                val end = System.currentTimeMillis()

                Imgcodecs.imwrite("images/output/${i - 1}_$mode.png", result)

                val confusion = Scorer.difference(result, mask).first

                Triple(mode, confusion, end - start)
            }
        }
    }
}
