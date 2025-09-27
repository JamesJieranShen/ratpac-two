#pragma once
#include <RAT/BoundedInterpolator.hh>
#include <RAT/DS/PMTInfo.hh>
#include <mimir/Cost.hh>
#include <mimir/Factory.hh>

namespace Mimir {
class PMTTypeTimeResidualNPE_PDF : public Cost {
 public:
  bool Configure(RAT::DBLinkPtr db_link) override;
  double operator()(const ParamSet& params) const override;
  void AddHit(const int pmtid, const RAT::FitterInputHandler& input_handler) override;

 protected:
  std::vector<int> hit_npes;
  double light_speed_internal, light_speed_external;
  double av_radius;
  std::map<int, RAT::BoundedInterpolator> tresid_pdf_splines;
  std::map<int, double> type_weights;
  RAT::DS::PMTInfo* pmt_info;
};
}  // namespace Mimir
