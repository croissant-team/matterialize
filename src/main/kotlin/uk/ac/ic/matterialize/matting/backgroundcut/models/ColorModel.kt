package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.Core
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Scalar

class ColorModel(private val bgImage: Image) {
    private val globalBgModel = GlobalBgModel(bgImage.flattened)
    private val pixelBgModel = PixelBgModel(bgImage)
    private val fgModel = FgModel(pixelBgModel)
    private var prevSegResult: SegmentationResult = SegmentationResult.empty(bgImage.mat.rows(), bgImage.mat.cols())

    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5

    init {
        assert(mixingFactor in 0.0..1.0)
    }

    fun updatePrevSegResult(prevSegmentationResult: SegmentationResult) {
        prevSegResult = prevSegmentationResult
    }

    fun mixProbs(image: FlatImage): Probs {
        val result = Mat()

        // equation (4)
        Core.addWeighted(
            globalBgModel.globalProbs(image).mat, mixingFactor,
            pixelBgModel.perPixelProbs(image).mat, 1 - mixingFactor, 0.0, result, CvType.CV_64FC1
        )
        return Probs(result)
    }

    fun colorTerms(image: FlatImage): ColorTerm {
        val bgEnergiesMat = Mat()
        Core.log(mixProbs(image).mat, bgEnergiesMat)
        Core.multiply(bgEnergiesMat, Scalar(-1.0), bgEnergiesMat)

        val fgEnergiesMat = Mat()
        Core.log(fgModel.globalProbs(image, prevSegResult).mat, fgEnergiesMat)
        Core.multiply(fgEnergiesMat, Scalar(-1.0), fgEnergiesMat)

        val bgEnergies = DoubleArray(image.numPixels())
        val fgEnergies = DoubleArray(image.numPixels())
        bgEnergiesMat.get(0, 0, bgEnergies)
        fgEnergiesMat.get(0, 0, fgEnergies)

        return ColorTerm(bgEnergies, fgEnergies)
    }
}