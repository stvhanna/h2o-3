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