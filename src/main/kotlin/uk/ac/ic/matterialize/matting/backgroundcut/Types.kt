package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.Core
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Size
import org.opencv.imgproc.Imgproc
import uk.ac.ic.matterialize.util.OpenCV.asDoubleArray

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
        // Note the CV_32_FC1 type is required by OpenCV to train GMMs
        mat.reshape(1, numPixels()).convertTo(result, CvType.CV_32FC1)
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
    private val data = mat.asDoubleArray()
    private val numChannels = mat.channels()

    fun at(pixelIndex: Int, channel: Int): Double {
        return data[pixelIndex * numChannels + channel]
    }
}

class Probs(internal val mat: Mat) {
    private val data: DoubleArray = mat.asDoubleArray()

    fun at(pixelIndex: Int): Double {
        return data[pixelIndex]
    }
}

class Image(internal val mat: Mat) {
     val flattened: FlatImage by lazy {
         val flatMat = mat.clone().reshape(1, mat.width() * mat.height())
         return@lazy FlatImage(flatMat)
    }

    // Returns array of per pixel variances, calculated in one the 8-neighborhood
    // of each pixel
    fun getPixelVariances(): PixelVariances {
        val meanOfSquare = Mat()
        val mean = Mat()
        val variances = Mat()
        val eightNeighborhoodSize = Size(3.0, 3.0)

        val matOfDoubles = Mat()

        // conver to double (CV_64FC3) to avoid overflowing ubytes (CV_8UC3)
        mat.convertTo(matOfDoubles, CvType.CV_64FC3)

        // blurring is equivalent to finding the mean
        Imgproc.blur(matOfDoubles.mul(matOfDoubles), meanOfSquare, eightNeighborhoodSize)
        Imgproc.blur(matOfDoubles, mean, eightNeighborhoodSize)

        val squareOfMean = mean.mul(mean)
        Core.subtract(meanOfSquare, squareOfMean, variances)
        val numPixels = mat.width() * mat.height()
        val flatVariances= variances.reshape(3, numPixels)


        return PixelVariances(flatVariances)
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