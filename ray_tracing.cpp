#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>

namespace py = pybind11;
using namespace pybind11::literals;

void trace_rays(
    const py::array_t<double>& e_mat_, 
    py::array_t<int64_t>& i_mat_, 
    py::array_t<int64_t>& tip_yf_mat_, py::array_t<int64_t>& tip_xf_mat_, 
    py::array_t<double>& z_mat_, 
    const int64_t img_h, const int64_t img_w, const int64_t i_ub, 
    const double viewer_x, const double viewer_y, const double viewer_z,
    const double viewer_azimuth, const double viewer_azimuth_w, 
    const double viewer_eleangle, const double viewer_eleangle_h
) {

    py::buffer_info      e_mat{     e_mat_.request()};
    py::buffer_info      i_mat{     i_mat_.request()};
    py::buffer_info tip_yf_mat{tip_yf_mat_.request()};
    py::buffer_info tip_xf_mat{tip_xf_mat_.request()};
    py::buffer_info      z_mat{     z_mat_.request()};

    const double* e_mat_ptr{static_cast< double*>(     e_mat.ptr)};
    int64_t*      i_mat_ptr{static_cast<int64_t*>(     i_mat.ptr)};
    int64_t* tip_yf_mat_ptr{static_cast<int64_t*>(tip_yf_mat.ptr)};
    int64_t* tip_xf_mat_ptr{static_cast<int64_t*>(tip_xf_mat.ptr)};
    double*       z_mat_ptr{static_cast< double*>(     z_mat.ptr)};

    for (int64_t idx_h{0}; idx_h < img_w; ++idx_h) {

        if (idx_h % 100 == 0) {
            // The following two lines mean the same.
            // py::print(idx_h, "end"_a=" ", "flush"_a=true); // todo: Is this flush correct ?
            py::print(idx_h, py::arg{"end"}=" ", py::arg{"flush"}=true); // todo: Is this flush correct ?
        }

        const double az{viewer_azimuth - viewer_azimuth_w / 2. + idx_h * (viewer_azimuth_w / (img_w - 1))};
        const double dire_x{cos((90. - az) * M_PI / 180.)};
        const double dire_y{sin((90. - az) * M_PI / 180.)};
        int64_t i{0};

        for (int64_t idx_v{0}; idx_v < img_h; ++idx_v) {

            const double eleang{viewer_eleangle - viewer_eleangle_h / 2. + idx_v * (viewer_eleangle_h / (img_h - 1))};
            
            while (i < i_ub) {

                const double dire_z{sin(eleang * M_PI / 180.)};
                const double dist{i * 1.};
                const double tip_x{viewer_x + dire_x * dist};
                const double tip_y{viewer_y + dire_y * dist};
                const double tip_z{viewer_z + dire_z * dist * 20.};
                const int64_t tip_xf{static_cast<int64_t>(floor(tip_x))}; // todo: dtype
                const int64_t tip_xc{static_cast<int64_t>( ceil(tip_x))};
                const int64_t tip_yf{static_cast<int64_t>(floor(tip_y))};
                const int64_t tip_yc{static_cast<int64_t>( ceil(tip_y))};

                if (tip_yf < 0 or tip_yc >= e_mat.shape[0] or tip_xf < 0 or tip_xc >= e_mat.shape[1]) {
                    i = i_ub;
                    break;
                }

                const double z{
                    (e_mat_ptr[tip_yf * e_mat.shape[1] + tip_xf] * (tip_xc - tip_x) + e_mat_ptr[tip_yf * e_mat.shape[1] + tip_xc] * (tip_x - tip_xf)) * (tip_yc - tip_y) + 
                    (e_mat_ptr[tip_yc * e_mat.shape[1] + tip_xf] * (tip_xc - tip_x) + e_mat_ptr[tip_yc * e_mat.shape[1] + tip_xc] * (tip_x - tip_xf)) * (tip_y - tip_yf)
                };
                if (z >= tip_z) {
                    i_mat_ptr     [idx_v * img_w + idx_h] = i;
                    tip_yf_mat_ptr[idx_v * img_w + idx_h] = tip_yf;
                    tip_xf_mat_ptr[idx_v * img_w + idx_h] = tip_xf;
                    z_mat_ptr     [idx_v * img_w + idx_h] = z;
                    break;
                }

                ++i;
            }
        }
    }

    // The following two lines mean the same.
    // py::print("flush"_a=true); // todo: Is this flush correct ?
    py::print(py::arg{"flush"}=true); // todo: Is this flush correct ?

    return;
}

PYBIND11_MODULE(ray_tracing_cpp, m) {
    m.def("trace_rays", &trace_rays, "");
}
