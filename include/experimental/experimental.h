/*
 * This file is part of Quantum++.
 *
 * MIT License
 *
 * Copyright (c) 2013 - 2019 Vlad Gheorghiu (vgheorgh@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * \file experimental/experimental.h
 * \brief Experimental/test functions/classes
 */

#ifndef EXPERIMENTAL_EXPERIMENTAL_H_
#define EXPERIMENTAL_EXPERIMENTAL_H_

namespace qpp {
/**
 * \namespace qpp::experimental
 * \brief Experimental/test functions/classes, do not use or modify
 */
namespace experimental {

// TODO: add a quantum instruction pointer and a measurement instruction pointer
// TODO: perform exception checking before run() (such as wrong idx on apply)

class QCircuit : public IDisplay {
    idx nq_;                             ///< number of qudits
    idx nc_;                             ///< number of classical "dits"
    idx d_;                              ///< dimension
    ket psi_;                            ///< state vector
    std::vector<idx> dits_;              ///< classical dits
    std::vector<double> probs_;          ///< measurement probabilities
    std::vector<idx> measurement_steps_; ///< keeps track of where the
                                         ///< measurement take place
    std::string name_;                   ///< optional circuit name
    std::vector<idx> subsys_;            ///< keeps track of the subsystem
                                         ///< relabeling after measurements

    const idx idx_inf = static_cast<idx>(-1); ///< largest idx
    /**
     * \brief Type of operation being executed at one step
     */
    enum class GateType {
        NONE, ///< signals no gate

        SINGLE, ///< unitary gate on a single qudit

        TWO, ///< unitary gate on 2 qudits

        THREE, ///< unitary gate on 3 qudits

        CUSTOM, ///< custom gate on multiple qudits

        FAN, ///< same unitary gate on multiple qudits

        SINGLE_CTRL_SINGLE_TARGET, ///< controlled 1 qudit unitary gate with
                                   ///< one control and one target

        SINGLE_CTRL_MULTIPLE_TARGET, ///< controlled 1 qudit unitary gate with
                                     ///< one control and multiple targets

        MULTIPLE_CTRL_SINGLE_TARGET, ///< controlled 1 qudit unitary gate with
                                     ///< multiple controls and single target

        MULTIPLE_CTRL_MULTIPLE_TARGET, ///< controlled 1 qudit unitary gate with
                                       ///< multiple controls and multiple
                                       ///< targets

        CUSTOM_CTRL, ///< custom controlled gate with multiple
                     ///< controls and multiple targets

        SINGLE_cCTRL_SINGLE_TARGET, ///< controlled 1 qudit unitary gate with
        ///< one classical control and one target

        SINGLE_cCTRL_MULTIPLE_TARGET, ///< controlled 1 qudit unitary gate with
        ///< one classical control and multiple targets

        MULTIPLE_cCTRL_SINGLE_TARGET, ///< controlled 1 qudit unitary gate with
        ///< multiple classical controls and single target

        MULTIPLE_cCTRL_MULTIPLE_TARGET, ///< controlled 1 qudit unitary gate
        ///< with multiple classical controls and multiple targets

        CUSTOM_cCTRL, ///< custom controlled gate with multiple
                      ///< controls and multiple targets
    };
    enum class MeasureType {
        NONE, ///< signals no measurement

        MEASURE_Z, ///< Z measurement of single qudit

        MEASURE_V, ///< measurement of single qudit in the orthonormal basis
                   ///< or rank-1 POVM specified by the columns of matrix \a V

        MEASURE_KS, ///< generalized measurement of single qudit with Kraus
                    ///< operators Ks
    };

    /**
     * \brief One step consisting of unitary gates in the circuit
     */
    struct GateStep {
        GateType gate_type_ = GateType::NONE; ///< gate type
        cmat gate_;                           ///< gate
        std::vector<idx> ctrl_;               ///< control
        std::vector<idx> target_; ///< target where the gate is being applied
        std::string name_;        ///< custom name of the step
        GateStep() = default;
        GateStep(GateType gate_type, const cmat& gate,
                 const std::vector<idx>& ctrl, const std::vector<idx>& target,
                 const std::string& name = "")
            : gate_type_{gate_type}, gate_{gate}, ctrl_{ctrl}, target_{target},
              name_{name} {}
    };

    struct MeasureStep {
        MeasureType measurement_type_ = MeasureType::NONE; ///< measurement type
        std::vector<cmat> mats_;  ///< matrix/matrices being measured
        std::vector<idx> target_; ///< target where the measurement is applied
        std::string name_;        ///< custom name of the step
        MeasureStep() = default;
        MeasureStep(MeasureType measurement_type, const std::vector<cmat>& mats,
                    const std::vector<idx>& target,
                    const std::string& name = "")
            : measurement_type_{measurement_type}, mats_{mats}, target_{target},
              name_{name} {}
    };

    friend std::ostream& operator<<(std::ostream& os,
                                    const GateType& gate_type) {
        switch (gate_type) {
        case GateType::NONE:
            return os << "GATE NONE";
        case GateType::SINGLE:
            return os << "SINGLE";
        case GateType::TWO:
            return os << "TWO";
        case GateType::THREE:
            return os << "THREE";
        case GateType::FAN:
            return os << "FAN";
        case GateType::CUSTOM:
            return os << "CUSTOM";
        case GateType::SINGLE_CTRL_SINGLE_TARGET:
            return os << "SINGLE_CTRL_SINGLE_TARGET";
        case GateType::SINGLE_CTRL_MULTIPLE_TARGET:
            return os << "SINGLE_CTRL_MULTIPLE_TARGET";
        case GateType::MULTIPLE_CTRL_SINGLE_TARGET:
            return os << "MULTIPLE_CTRL_SINGLE_TARGET";
        case GateType::MULTIPLE_CTRL_MULTIPLE_TARGET:
            return os << "MULTIPLE_CTRL_MULTIPLE_TARGET";
        case GateType::CUSTOM_CTRL:
            return os << "CUSTOM_CTRL";
        case GateType::SINGLE_cCTRL_SINGLE_TARGET:
            return os << "SINGLE_cCTRL_SINGLE_TARGET";
        case GateType::SINGLE_cCTRL_MULTIPLE_TARGET:
            return os << "SINGLE_cCTRL_MULTIPLE_TARGET";
        case GateType::MULTIPLE_cCTRL_SINGLE_TARGET:
            return os << "MULTIPLE_cCTRL_SINGLE_TARGET";
        case GateType::MULTIPLE_cCTRL_MULTIPLE_TARGET:
            return os << "MULTIPLE_cCTRL_MULTIPLE_TARGET";
        case GateType::CUSTOM_cCTRL:
            return os << "CUSTOM_cCTRL";
        }
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const MeasureType& measure_type) {
        switch (measure_type) {
        case MeasureType::NONE:
            return os << "\t|> MEASURE NONE";
        case MeasureType::MEASURE_Z:
            return os << "\t|> MEASURE_Z";
        case MeasureType::MEASURE_V:
            return os << "\t|> MEASURE_V";
        case MeasureType::MEASURE_KS:
            return os << "\t|> MEASURE_KS";
        }
    }

    ///< quantum circuit representation
    std::vector<GateStep> gates_;
    std::vector<MeasureStep> measurements_;

  public:
    QCircuit(idx nq, idx nc = 0, idx d = 2, const std::string& name = "")
        : nq_{nq}, nc_{nc}, d_{d}, psi_{st.zero(nq_, d_)}, dits_(nc_, 0),
          probs_(nc_, 0), name_{name}, subsys_(nq, 0) {
        std::iota(std::begin(subsys_), std::end(subsys_), 0);
    }

    // getters
    // qudit i was measured
    bool is_measured(idx i) const { return (subsys_[i] == idx_inf); }

    // get the already measured qudits
    std::vector<idx> get_measured() const {
        std::vector<idx> result;
        for (idx i = 0; i < nq_; ++i)
            if (subsys_[i] == idx_inf)
                result.emplace_back(i);

        return result;
    }

    ket get_psi() const { return psi_; }
    // end getters

  private:
    // mark qudit i as measured then re-label accordingly the remaining
    // non-measured qudits
    // TODO: set to no-op if measured before?!
    void mark_as_measured_(idx i) {
        if (is_measured(i))
            throw qpp::exception::QuditAlreadyMeasured(
                "qpp::QCircuit::mark_as_measured_()");
        subsys_[i] = idx_inf; // set qudit i to measured state
        for (idx m = i; m < nq_; ++m) {
            if (!is_measured(m)) {
                --subsys_[m];
            }
        }
    }

    // giving a vector of non-measured qudits, get their relative position of
    // the wrt the measured qudits
    std::vector<idx> get_relative_pos_(std::vector<idx> v) {
        idx vsize = v.size();
        for (idx i = 0; i < vsize; ++i) {
            if (is_measured(v[i]))
                throw qpp::exception::QuditAlreadyMeasured(
                    "qpp::QCircuit::get_relative_pos_()");
            v[i] = subsys_[v[i]];
        }
        return v;
    }

  public:
    // single gate single qudit
    void apply(const cmat& U, idx i, const std::string& name = "") {
        gates_.emplace_back(GateType::SINGLE, U, std::vector<idx>{},
                            std::vector<idx>{i}, name);
    }
    // single gate 2 qudits
    void apply(const cmat& U, idx i, idx j, const std::string& name = "") {
        gates_.emplace_back(GateType::TWO, U, std::vector<idx>{},
                            std::vector<idx>{i, j}, name);
    }
    // single gate 3 qudits
    void apply(const cmat& U, idx i, idx j, idx k,
               const std::string& name = "") {
        gates_.emplace_back(GateType::THREE, U, std::vector<idx>{},
                            std::vector<idx>{i, j, k}, name);
    }

    // multiple qudits same gate
    void apply_many(const cmat& U, const std::vector<idx>& target,
                    const std::string& name = "") {
        gates_.emplace_back(GateType::FAN, U, std::vector<idx>{}, target, name);
    }

    // custom gate
    void apply(const cmat& U, const std::vector<idx>& target,
               const std::string& name = "") {
        gates_.emplace_back(GateType::CUSTOM, U, std::vector<idx>{}, target,
                            name);
    }

    // single ctrl single target
    void CTRL(const cmat& U, idx ctrl, idx target,
              const std::string& name = "") {
        gates_.emplace_back(GateType::SINGLE_CTRL_SINGLE_TARGET, U,
                            std::vector<idx>{ctrl}, std::vector<idx>{target},
                            name);
    }

    // single ctrl multiple target
    void CTRL(const cmat& U, idx ctrl, const std::vector<idx>& target,
              const std::string& name = "") {
        gates_.emplace_back(GateType::SINGLE_CTRL_MULTIPLE_TARGET, U,
                            std::vector<idx>{ctrl}, target, name);
    }

    // multiple ctrl single target
    void CTRL(const cmat& U, const std::vector<idx>& ctrl, idx target,
              const std::string& name = "") {
        gates_.emplace_back(GateType::MULTIPLE_CTRL_SINGLE_TARGET, U, ctrl,
                            std::vector<idx>{target}, name);
    }

    // multiple ctrl multiple target
    void CTRL(const cmat& U, const std::vector<idx>& ctrl,
              const std::vector<idx>& target, const std::string& name = "") {
        gates_.emplace_back(GateType::MULTIPLE_CTRL_MULTIPLE_TARGET, U, ctrl,
                            std::vector<idx>{target}, name);
    }

    //  custom controlled gate with multiple controls and multiple targets
    void CTRL_custom(const cmat& U, const std::vector<idx>& ctrl,
                     const std::vector<idx>& target,
                     const std::string& name = "") {
        gates_.emplace_back(GateType::CUSTOM_CTRL, U, ctrl, target, name);
    }

    // single ctrl single target
    void cCTRL(const cmat& U, idx ctrl, idx target,
               const std::string& name = "") {
        gates_.emplace_back(GateType::SINGLE_cCTRL_SINGLE_TARGET, U,
                            std::vector<idx>{ctrl}, std::vector<idx>{target},
                            name);
    }

    // single ctrl multiple target
    void cCTRL(const cmat& U, idx ctrl, const std::vector<idx>& target,
               const std::string& name = "") {
        gates_.emplace_back(GateType::SINGLE_cCTRL_MULTIPLE_TARGET, U,
                            std::vector<idx>{ctrl}, target, name);
    }

    // multiple ctrl single target
    void cCTRL(const cmat& U, const std::vector<idx>& ctrl, idx target,
               const std::string& name = "") {
        gates_.emplace_back(GateType::MULTIPLE_cCTRL_SINGLE_TARGET, U, ctrl,
                            std::vector<idx>{target}, name);
    }

    // multiple ctrl multiple target
    void cCTRL(const cmat& U, const std::vector<idx>& ctrl,
               const std::vector<idx>& target, const std::string& name = "") {
        gates_.emplace_back(GateType::MULTIPLE_cCTRL_MULTIPLE_TARGET, U, ctrl,
                            std::vector<idx>{target}, name);
    }

    //  custom controlled gate with multiple controls and multiple targets
    void cCTRL_custom(const cmat& U, const std::vector<idx>& ctrl,
                      const std::vector<idx>& target,
                      const std::string& name = "") {
        gates_.emplace_back(GateType::CUSTOM_cCTRL, U, ctrl, target, name);
    }

    // Z measurement of single qudit
    void measureZ(idx i, idx c_reg, const std::string& name = "") {
        // EXCEPTION CHECKS

        // measuring non-existing qudit
        if (i >= nq_)
            throw qpp::exception::OutOfRange("qpp::QCircuit::measureZ()");
        // measuring an already measured qudit
        if (is_measured(i))
            throw qpp::exception::QuditAlreadyMeasured(
                "qpp::QCircuit::measureZ()");
        // trying to put the result into an non-existing classical slot
        if (c_reg >= dits_.size())
            throw qpp::exception::OutOfRange("qpp::QCircuit::measureZ()");
        // END EXCEPTION CHECKS

        measurements_.emplace_back(MeasureType::MEASURE_Z, std::vector<cmat>{},
                                   std::vector<idx>{i}, name);
        measurement_steps_.emplace_back(gates_.size());
        mark_as_measured_(i);
        /*try {
        } catch (std::exception& e) {
            std::cerr << "IN qpp::QCircuit::measureZ()\n";
            throw;
        }*/
    }

    // measurement of single qudit in the orthonormal basis or rank-1 POVM
    // specified by the columns of matrix V
    void measureV(const cmat& V, idx i, idx c_reg,
                  const std::string& name = "") {
        // EXCEPTION CHECKS

        // measuring non-existing qudit
        if (i >= nq_)
            throw qpp::exception::OutOfRange("qpp::QCircuit::measureV()");
        // measuring an already measured qudit
        if (is_measured(i))
            throw qpp::exception::QuditAlreadyMeasured(
                "qpp::QCircuit::measureV()");
        // trying to put the result into an non-existing classical slot
        if (c_reg >= dits_.size())
            throw qpp::exception::OutOfRange("qpp::QCircuit::measureV()");
        // END EXCEPTION CHECKS

        measurements_.emplace_back(MeasureType::MEASURE_V, std::vector<cmat>{V},
                                   std::vector<idx>{i}, name);
        measurement_steps_.emplace_back(gates_.size());
        mark_as_measured_(i);
    }

    // generalized measurement of single qudit with Kraus operators Ks
    void measureKs(const std::vector<cmat>& Ks, idx i, idx c_reg,
                   const std::string& name = "") {
        // EXCEPTION CHECKS

        // measuring non-existing qudit
        if (i >= nq_)
            throw qpp::exception::OutOfRange("qpp::QCircuit::measureKs()");
        // measuring an already measured qudit
        if (is_measured(i))
            throw qpp::exception::QuditAlreadyMeasured(
                "qpp::QCircuit::measureKs()");
        // trying to put the result into an non-existing classical slot
        if (c_reg >= dits_.size())
            throw qpp::exception::OutOfRange("qpp::QCircuit::measureKs()");
        // END EXCEPTION CHECKS

        measurements_.emplace_back(MeasureType::MEASURE_KS, Ks,
                                   std::vector<idx>{i}, name);
        measurement_steps_.emplace_back(gates_.size());
        mark_as_measured_(i);
    }

    std::ostream& display(std::ostream& os) const override {
        os << "nq = " << nq_ << ", nc = " << nc_ << ", d = " << d_;

        if (name_ != "") // if the circuit is named
            os << ", name = \"" << name_ << "\"\n";
        else
            os << ", name = \"\"\n";

        idx gates_size = gates_.size();
        idx measurements_size = measurement_steps_.size();

        idx measurement_ip = 0; // measurement instruction pointer
        for (idx i = 0; i <= gates_size; ++i) {
            // check for measurements before gates
            if (measurements_size != 0) {
                idx current_measurement_step =
                    measurement_steps_[measurement_ip];
                // we have a measurement
                if (current_measurement_step == i) {
                    while (measurement_steps_[measurement_ip] ==
                           current_measurement_step) {
                        os << measurements_[measurement_ip].measurement_type_
                           << ", "
                           << "\"" << measurements_[measurement_ip].name_
                           << "\""
                           << ", ";
                        os << disp(measurements_[measurement_ip].target_, ",");
                        os << '\n';
                        ++measurement_ip;
                    }
                }
            }
            // check for gates
            if (i < gates_size) {
                os << gates_[i].gate_type_ << ", "
                   << "\"" << gates_[i].name_ << "\""
                   << ", ";
                if (gates_[i].gate_type_ >=
                    GateType ::SINGLE_CTRL_SINGLE_TARGET)
                    os << disp(gates_[i].ctrl_, ",") << ", ";
                os << disp(gates_[i].target_, ",");
                os << '\n';
            }
        }

        os << std::boolalpha;
        os << "measured: " << disp(get_measured(), ",") << '\n';
        os << std::noboolalpha;
        os << "dits: " << disp(dits_, ",") << '\n';
        os << "probs: " << disp(probs_, ",") << '\n';

        os << "measurement steps (assumed after gate steps): ";
        std::cout << disp(measurement_steps_, ",") << '\n';

        os << "updated subsystems: ";
        std::cout << disp(subsys_, ",") << '\n';

        return os;
    }

    void run() {
        idx gates_size = gates_.size();
        idx measurements_size = measurement_steps_.size();

        idx measurement_ip = 0; // measurement instruction pointer
        for (idx i = 0; i <= gates_size; ++i) {
            // check for measurements before gates
            if (measurements_size != 0) {
                idx current_measurement_step =
                    measurement_steps_[measurement_ip];
                // we have a measurement
                if (current_measurement_step == i) {
                    while (measurement_steps_[measurement_ip] ==
                           current_measurement_step) {

                        /* measurement code here */
                        switch (measurements_[i].measurement_type_) {
                        case MeasureType::NONE:
                            break;
                        case MeasureType::MEASURE_Z:
                            break;
                        case MeasureType::MEASURE_V:
                            break;
                        case MeasureType::MEASURE_KS:
                            break;
                        }

                        ++measurement_ip;
                    }
                }
            }

            // gates code here
            if (i < gates_size) {
                /* gates code here */
                std::vector<idx> relative_pos =
                    get_relative_pos_(gates_[i].target_);
                switch (gates_[i].gate_type_) {
                case GateType::NONE:
                    break;
                case GateType::SINGLE:
                case GateType::TWO:
                case GateType::THREE:
                case GateType::CUSTOM:
                    psi_ = qpp::apply(psi_, gates_[i].gate_, relative_pos, d_);
                    break;
                case GateType::FAN:
                    for (idx m = 0; m < gates_[i].target_.size(); ++m)
                        psi_ = qpp::apply(psi_, gates_[i].gate_,
                                          {relative_pos[m]}, d_);
                    break;
                case GateType::SINGLE_CTRL_SINGLE_TARGET:
                    break;
                case GateType::SINGLE_CTRL_MULTIPLE_TARGET:
                    break;
                case GateType::MULTIPLE_CTRL_SINGLE_TARGET:
                    break;
                case GateType::MULTIPLE_CTRL_MULTIPLE_TARGET:
                    break;
                case GateType::CUSTOM_CTRL:
                    break;
                case GateType::SINGLE_cCTRL_SINGLE_TARGET:
                    break;
                case GateType::SINGLE_cCTRL_MULTIPLE_TARGET:
                    break;
                case GateType::MULTIPLE_cCTRL_SINGLE_TARGET:
                    break;
                case GateType::MULTIPLE_cCTRL_MULTIPLE_TARGET:
                    break;
                case GateType::CUSTOM_cCTRL:
                    break;
                }
            }
        }
    }
}; // namespace experimental

} // namespace experimental
} /* namespace qpp */

#endif /* EXPERIMENTAL_EXPERIMENTAL_H_ */
