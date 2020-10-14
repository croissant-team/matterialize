package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.Mat
import org.opencv.core.Scalar

interface Matter {

    fun foregroundMask(videoFrame: Mat): Mat

    fun greenscreen(videoFrame: Mat): Mat {
        val greenscreen = Mat(videoFrame.size(), videoFrame.type(), Scalar(0.0, 255.0, 0.0))
        return changeBackground(videoFrame, greenscreen)
    }

    fun changeBackground(videoFrame: Mat, newBackground: Mat): Mat {
        val fgMask = foregroundMask(videoFrame)
        val bgMask = Mat()
        Core.bitwise_not(fgMask, bgMask)
        val bg = Mat()
        val fg = Mat()
        videoFrame.copyTo(fg, fgMask)
        newBackground.copyTo(bg, bgMask)

        val result = Mat()
        Core.add(fg, bg, result)

        return result
    }

    fun removeBackground(videoFrame: Mat): Mat {
        val result = Mat()
        val fgMask = foregroundMask(videoFrame)
        videoFrame.copyTo(result, fgMask)

        return result
    }
}
