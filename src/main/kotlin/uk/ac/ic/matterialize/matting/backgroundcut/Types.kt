package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.Core
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Size
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc

class SegmentationResult(val image: Image, val bgMask: FlatMask) {
    companion object {
        fun empty(rows: Int, cols: Int): SegmentationResult {
            return SegmentationResult(
                Image(Mat.zeros(rows, cols, CvType.CV_8UC3)),
                FlatMask(Mat.zeros(rows * cols, 1, CvType.CV_8UC1))
            )
        }
    }
}

class FlatImage(internal val mat: Mat) {
    private val data: UByteArray
    private val numChannels = 3

    init {
        assert(mat.width() == numChannels)
        val tempData = ByteArray(numPixels() * numChannels)
        mat.get(0, 0, tempData)
        data = tempData.toUByteArray()
    }

    fun get(pixelIndex: Int, channel: Int): UByte {
        return data[pixelIndex * numChannels + channel]
    }

    fun numPixels(): Int {
        return mat.height()
    }

    fun toSamples(): Mat {
        val result = Mat()
        mat.convertTo(result, CvType.CV_32FC1)
        return result
    }
}

class FlatMask(internal val mat: Mat) {
    private val data: UByteArray
    private val numChannels = 1


    init {
        assert(mat.width() == numChannels)
        val tempData = ByteArray(mat.width() * numChannels)
        mat.get(0, 0, tempData)
        data = tempData.toUByteArray()
    }

    fun get(pixelIndex: Int): UByte {
        return data[pixelIndex]
    }
}

class PixelVariances(internal val mat: Mat) {
    fun at(pixelIndex: Int, channel: Int): Double {
        return mat.get(pixelIndex, channel)[0]
    }
}

class Probs(internal val mat: Mat) {
    fun at(pixelIndex: Int): Double {
        return mat.get(pixelIndex, 0)[0]
    }
}

class Image(internal val mat: Mat) {
     val flattened: FlatImage by lazy {
         val flatMat = mat.clone().reshape(1, mat.width() * mat.height())
         return@lazy FlatImage(flatMat)
    }

    // Returns array of per pixel variances, calculated in one the 8-neighborhood
    // of each pixel
    fun getPerPixelVariances(): PixelVariances {
        val meanOfSquare = Mat()
        val mean = Mat()
        val variances = Mat()
        val eightNeighborhoodSize = Size(3.0, 3.0)

        val floatMat = Mat()

        mat.convertTo(floatMat, CvType.CV_32FC3, 1 / 255.0)

        // blurring is equivalent to finding the mean
        Imgproc.blur(floatMat.mul(floatMat), meanOfSquare, eightNeighborhoodSize)
        Imgcodecs.imwrite("meanOfSquare.png", meanOfSquare)

        Imgproc.blur(floatMat, mean, eightNeighborhoodSize)
        val squareOfMean = mean.mul(mean)
        Imgcodecs.imwrite("squareOfMean.png", squareOfMean)

        Core.subtract(meanOfSquare, squareOfMean, variances)
        val numPixels = mat.width() * mat.height()

        return PixelVariances(variances.reshape(1, numPixels))
    }

    fun resized(rows: Int, cols: Int): Image {
        val result = Mat()
        Imgproc.resize(this.mat, result, Size(rows.toDouble(), cols.toDouble()))
        return Image(result)
    }

    fun downscaled(factor: Int): Image {
        val width = this.mat.width() / factor
        val height = this.mat.height() / factor

        return this.resized(width, height)
    }

    fun upscaled(factor: Int): Image {
        val width = this.mat.width() * factor
        val height = this.mat.height() * factor

        return this.resized(width, height)
    }
}

class ColorTerm(bgEnergies: DoubleArray, fgEnergies: DoubleArray)