##
## Word2Vec
##
## Create a word2vec model.
##
##  @param trainingH2OFrame - H2O Frame with a String column containing training words
##  @param minWordFreq - Minimum number of occurences of a word to be considered for training
##  @param vecSize - Size of word vectors
##  @param winSize - Size of word window
##  @param sentSampleRate - Sampling rate in sentences to generate new n-grams
##  @param initLearningRate - Starting alpha value.  This tempers the effect of progressive information as learning progresses.
##  @param epochs - Number of iterations data is run through.
h2o.word2vec <- function(trainingH2OFrame, minWordFreq, vecSize, windowSize, sentSampleRate, initLearningRate, epochs) {

  # param checking
  if (!is.H2OFrame(trainingH2OFrame)) stop("`data` must be an H2OFrame object")
  if (missing(vecSize) || !is.numeric(vecSize)) stop("`vecSize` must be numeric")
  if (missing(windowSize) || !is.numeric(windowSize)) stop("`windowSize` must be numeric")
  if (missing(sentSampleRate) || !is.numeric(sentSampleRate)) stop("`sentSampleRate` must be numeric")
  if (missing(initLearningRate) || !is.numeric(initLearningRate)) stop("`initLearningRate` must be numeric")
  if (missing(epochs) || !is.numeric(epochs)) stop("`epochs` must be numeric")

  params <- list(training_frame = trainingH2OFrame,
                 wordModel = "SkipGram",
                 normModel = "HSM",
                 minWordFreq = minWordFreq,
                 vecSize = vecSize,
                 windowSize = windowSize,
                 sentSampleRate = sentSampleRate,
                 initLearningRate = initLearningRate,
                 epochs = epochs)

  .h2o.modelJob("word2vec", params, h2oRestApiVersion = 3)
}

##
## Find synonyms using a word2vec model.
##
##  @param word2vec: A word2vec model.
##  @param target: Single word to find synonyms for.
##  @param count: The top `count` synonyms will be returned.
##
h2o.findSynonyms <- function(word2vec, target, count = 20) {
    if (missing(target)) stop("`target` must be specified")
    if (!is.character(target)) stop("`target` must be character")
    if (missing(count)) stop("`count` must be specified")
    if (!is.numeric(count)) stop("`count` must be numeric")

    res <- .h2o.__remoteSend(method="GET", "Word2VecSynonyms", model = word2vec@model_id,
    target = target, count = count)
    fr <- data.frame(synonyms = res$words, scores = res$scores)
    fr[with(fr, order(res$scores, decreasing = TRUE)),]
}