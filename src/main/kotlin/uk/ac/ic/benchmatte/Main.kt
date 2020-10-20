package uk.ac.ic.benchmatte

import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.imgcodecs.Imgcodecs
import uk.ac.ic.benchmatte.util.Compositor
import uk.ac.ic.benchmatte.util.Scorer
import uk.ac.ic.matterialize.matting.BackgroundNegationMatter
import uk.ac.ic.matterialize.matting.Matter

fun main() {
    nu.pattern.OpenCV.loadLocally()

    val targetMask = Compositor.loadFile("", CvType.CV_8U)
    val background = Compositor.loadFile("")
    val foreground = Compositor.loadFile("")

    val matter: Matter = BackgroundNegationMatter(background)

    val composed = Compositor.compose(background, foreground, targetMask)

    val resultMask = matter.foregroundMask(composed)

    println("resultMask : ${resultMask.type()}")
    println("targetMask : ${targetMask.type()}")

    Imgcodecs.imwrite("_composed.png", composed)
    Imgcodecs.imwrite("_resultMask.png", resultMask)
    Imgcodecs.imwrite("_targetMask.png", targetMask)

    val (_, delta) = Scorer.difference(targetMask, resultMask)

    Imgcodecs.imwrite("_delta.png", delta)

    println(Scorer.percentage(resultMask, targetMask))
}
