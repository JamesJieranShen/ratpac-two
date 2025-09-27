#include <TVector3.h>

#include <RAT/DS/RunStore.hh>
#include <RAT/LightPathCalculator.hh>
#include <RAT/Log.hh>
#include <limits>
#include <mimir/PMTTypeTimeResidualNPE_PDF.hh>
#include <numeric>

namespace Mimir {
bool PMTTypeTimeResidualNPE_PDF::Configure(RAT::DBLinkPtr db_link) {
  std::vector<double> binning = db_link->GetDArray("binning");
  std::vector<int> pmt_types = db_link->GetIArray("pmt_types");
  std::vector<double> _type_weights = db_link->GetDArray("type_weights");
  RAT::Log::Assert(pmt_types.size() == _type_weights.size(),
                   "mimir::PMTTypeTimeResidualNPE_PDF: pmt_types and type_weights must have the same size.");
  pmt_info = RAT::DS::RunStore::GetCurrentRun()->GetPMTInfo();
  double bin_width = binning.at(1) - binning.at(0);
  tresid_pdf_splines.clear();
  type_weights.clear();
  for (size_t idx = 0; idx < pmt_types.size(); ++idx) {
    int pmt_type = pmt_types.at(idx);
    type_weights[pmt_type] = _type_weights.at(idx);
    std::vector<double> histvals = db_link->GetDArray("hist_" + std::to_string(pmt_type));
    for (double val : histvals) {
      if (val == 0)
        RAT::Log::Die("mimir::PMTTypeTimeResidualNPE_PDF: PDF histogram for PMT type " + std::to_string(pmt_type) +
                      " has zero entries. Cannot normalize.");
    }
    double norm = std::accumulate(histvals.begin(), histvals.end(), 0.0) * bin_width;
    for (double& val : histvals) {
      val /= norm;
    }
    tresid_pdf_splines.try_emplace(pmt_type, binning, histvals, ROOT::Math::Interpolation::kCSPLINE);
  }
  light_speed_internal = db_link->GetD("light_speed_in_medium");
  light_speed_external = db_link->GetD("light_speed_in_external_medium");
  av_radius = db_link->GetD("av_radius");
  return true;
}

void PMTTypeTimeResidualNPE_PDF::AddHit(const int pmtid, const RAT::FitterInputHandler& input_handler) {
  Cost::AddHit(pmtid, input_handler);
  hit_npes.push_back(input_handler.GetNPEs(pmtid));
}

double PMTTypeTimeResidualNPE_PDF::operator()(const ParamSet& params) const {
  if (hit_pmtids.size() != hit_times.size() || hit_pmtids.size() != hit_charges.size()) {
    RAT::Log::Die("mimir::PMTTypeTimeResidualNPE_PDF: hit_pmtids, hit_times, and hit_charges must have the same size.");
  }
  if (hit_pmtids.empty()) {
    RAT::warn << "mimir::PMTTypeTimeResidualNPE_PDF: No hits set, returning 0." << newline;
    return 0.0;
  }
  if (hit_times.at(0) == INVALID) {
    RAT::Log::Die(
        "mimir::PMTTypeTimeResidualNPE_PDF: hit times were not provided. Cannot evaluate Time Residual PDF without hit "
        "times.");
  }
  ParamField position_time = params.position_time;
  std::vector<double> xyzt = position_time.active_values();
  if (xyzt.size() != 4) {
    RAT::Log::Die("mimir::PMTTypeTimeResidualNPE_PDF: position_time must have 4 active components (x, y, z, t).");
  }
  TVector3 vertex_position(xyzt.at(0), xyzt.at(1), xyzt.at(2));
  double vertex_time = xyzt.at(3);
  double result = 0.0;
  for (size_t ihit = 0; ihit < hit_pmtids.size(); ++ihit) {
    int pmtid = hit_pmtids.at(ihit);
    double time = hit_times.at(ihit);
    int npe = hit_npes.at(ihit);
    int pmt_type = pmt_info->GetType(pmtid);
    if (tresid_pdf_splines.count(pmt_type) == 0) {
      RAT::warn << "mimir::PMTTypeTimeResidualNPE_PDF: No spline for PMT type " << pmt_type << ". Skipping this hit."
                << newline;
      continue;
    }
    TVector3 pmt_position = pmt_info->GetPosition(pmtid);
    const RAT::BoundedInterpolator& spe_pdf_spline = tresid_pdf_splines.at(pmt_type);
    double weight = type_weights.at(pmt_type);
    double vtx_to_av = RAT::DistanceToSphere(vertex_position, pmt_position, av_radius);
    double av_to_pmt = (pmt_position - vertex_position).Mag() - vtx_to_av;
    double tof = vtx_to_av / light_speed_internal + av_to_pmt / light_speed_external;
    double tresid = time - vertex_time - tof;
    // use ordered statistics for npe > 1
    double likelihood;
    if (npe == 1)
      likelihood = spe_pdf_spline.Eval(tresid);
    else {
      double pdf = spe_pdf_spline.Eval(tresid);
      double rcdf = spe_pdf_spline.Integ(tresid, std::numeric_limits<double>::max());
      if (rcdf <= 0) {
        rcdf = 1e-10;
      }
      likelihood = npe * pdf * pow(rcdf, npe - 1);
    }
    if (likelihood == 0) {
      RAT::warn << "mimir::PMTTypeTimeResidualNPE_PDF: PDF evaluated to zero, cannot take log. Inputs are NPE = " << npe
                << " tresid = " << tresid << newline;
      RAT::warn << "  spe likelihood = " << spe_pdf_spline.Eval(tresid)
                << " cdf = " << spe_pdf_spline.Integ(std::numeric_limits<double>::lowest(), tresid) << newline;
      RAT::Log::Die("See above warning for details.");
    }

    result -= std::log(likelihood) * weight;
  }

  return result;
}

}  // namespace Mimir
MIMIR_REGISTER_TYPE(Mimir::Cost, Mimir::PMTTypeTimeResidualNPE_PDF, "PMTTypeTimeResidualNPE_PDF")
