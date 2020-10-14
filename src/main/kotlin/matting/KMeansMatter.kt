package matting

import org.opencv.core.Mat
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.COLOR_BGR2Lab
import org.opencv.imgproc.Imgproc.COLOR_Lab2BGR
import smile.clustering.KMeans
import smile.clustering.kmeans
import smile.math.MathEx

class KMeansMatter(background: Mat): Matter {
    private data class ClusterInfo(val variance: Double, val components: Array<DoubleArray>)
    private data class ClusteringInfo(val kMeans: KMeans, val clustersInfo: Map<Int, ClusterInfo>)

    private val NUM_CLUSTERS = 20
    private val THRESHOLD = 0.000003

    private val backgroundInfo: ClusteringInfo
    init {
        backgroundInfo = kMeansClustering(background)
    }

    private fun BGR2LAB(input: Mat): Mat {
        val result = Mat()
        Imgproc.cvtColor(input, result, COLOR_BGR2Lab)
        return result
    }

    private fun LAB2BGR(input: Mat): Mat {
        val result = Mat()
        Imgproc.cvtColor(input, result, COLOR_Lab2BGR)
        return result
    }

    private fun kMeansClustering(input: Mat): ClusteringInfo {
        val image = BGR2LAB(input)
        Imgcodecs.imwrite("blurandlab.png", image)
        val dataPoints: MutableList<DoubleArray> = ArrayList()

        for (y in 0 until image.height()) {
            for (x in 0 until image.width()) {
                val pixel = image.get(y, x)
                val blue = pixel[0]
                val green = pixel[1]
                val red = pixel[2]
                dataPoints.add(doubleArrayOf(blue, green, red))
            }
        }

        val kMeans: KMeans = kmeans(dataPoints.toTypedArray(), NUM_CLUSTERS)
        val clustersSeparation: MutableMap<Int, MutableList<DoubleArray>> = HashMap()

        for (dataPoint in dataPoints) {
            val belongsTo: Int = kMeans.predict(dataPoint)
            if (!clustersSeparation.containsKey(belongsTo)) {
                clustersSeparation[belongsTo] = ArrayList()
            }
            clustersSeparation[belongsTo]?.add(dataPoint)
        }

        val clustersInfo: MutableMap<Int, ClusterInfo> = HashMap()
        for (entry in clustersSeparation.entries) {
            val distances: DoubleArray = entry.value.map { point -> kMeans.distance(kMeans.centroids[entry.key], point) }.toDoubleArray()
            clustersInfo[entry.key] = ClusterInfo(MathEx.`var`(distances), entry.value.toTypedArray())
        }

        return ClusteringInfo(kMeans, clustersInfo.toMap())
    }

    // TODO remove or extract
    fun clusteringPaint(input: Mat): Mat {
        val image = BGR2LAB(input)
        Imgproc.cvtColor(input, image, COLOR_BGR2Lab)
        val clusteringInfo: ClusteringInfo = kMeansClustering(image)
        for (y in 0 until image.height()) {
            for (x in 0 until image.width()) {
                val pixel = image.get(y, x)
                val blue = pixel[0]
                val green = pixel[1]
                val red = pixel[2]

                val belongsTo: Int = clusteringInfo.kMeans.predict(doubleArrayOf(blue, green, red))
                val cluster = clusteringInfo.kMeans.centroids[belongsTo]
                image.put(y, x, cluster[0], cluster[1], cluster[2])
            }
        }
        return LAB2BGR(image)
    }

    // TODO Quote the paper properly
    // Used "Video Segmentation into Background and Foreground Using Simplified Mean Shift Filter and K-Means Clustering"
    override fun backgroundMask(input: Mat): Mat {
        val image = BGR2LAB(input)
        val mask = Mat(image.size(), image.type())
        for (y in 0 until image.height()) {
            for (x in 0 until image.width()) {
                val pixel = image.get(y, x)
                val blue = pixel[0]
                val green = pixel[1]
                val red = pixel[2]

                val dataPoint = doubleArrayOf(blue, green, red)
                val belongsTo: Int = backgroundInfo.kMeans.predict(dataPoint)
                val cluster = backgroundInfo.kMeans.centroids[belongsTo]
                val clusterInfo = backgroundInfo.clustersInfo.getValue(belongsTo)
                val dist = backgroundInfo.kMeans.distance(cluster, dataPoint)
                val ratio = dist / (clusterInfo.variance * clusterInfo.components.size)
                if (ratio > THRESHOLD) {
                    mask.put(y, x, 255.0, 255.0, 255.0)
                } else {
                    mask.put(y, x, 0.0, 0.0, 0.0)
                }
            }
        }
        return mask
    }
}