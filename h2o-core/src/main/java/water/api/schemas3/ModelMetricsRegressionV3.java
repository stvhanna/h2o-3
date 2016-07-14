package water.api.schemas3;

import hex.ModelMetricsRegression;
import water.api.API;

public class ModelMetricsRegressionV3<I extends ModelMetricsRegression, S extends ModelMetricsRegressionV3<I, S>> extends ModelMetricsBaseV3<I, S> {
  @API(help="The R^2 for this scoring run.", direction=API.Direction.OUTPUT)
  public double r2;

  @API(help="The mean residual deviance for this scoring run.", direction=API.Direction.OUTPUT)
  public double mean_residual_deviance;

  @API(help="The mean absolute error for this scoring run.", direction=API.Direction.OUTPUT)
  public double mae;

  @Override
  public S fillFromImpl(I modelMetrics) {
    super.fillFromImpl(modelMetrics);
    r2 = modelMetrics.r2();
    mae = modelMetrics._mean_absolute_error;
    return (S) this;
  }
}
