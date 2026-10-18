#include "scenario/scenario_engine.hpp"

#include <stdexcept>
#include <algorithm>

namespace etwin::scenario {

ScenarioEngine::ScenarioEngine() : rng_(std::random_device{}()) {}

void ScenarioEngine::register_model(
    const std::string& name,
    std::function<std::unique_ptr<etwin::models::ModelModule>()> creator) {
    model_registry_[name] = std::move(creator);
}

ScenarioResult ScenarioEngine::run(const ScenarioConfig& config) {
    ScenarioResult result;
    result.scenario_id = config.scenario_id;
    result.success = false;
    result.status = "initializing";

    if (config.start_time >= config.end_time) {
        result.errors.push_back("Start time must be before end time");
        result.status = "failed";
        return result;
    }

    std::vector<std::unique_ptr<etwin::models::ModelModule>> models;
    for (const auto& mc : config.models) {
        auto it = model_registry_.find(mc.model_name);
        if (it == model_registry_.end()) {
            result.errors.push_back("Unknown model: " + mc.model_name);
            result.status = "failed";
            return result;
        }
        models.push_back(it->second());
    }

    etwin::models::ExecutionContext ctx;
    ctx.current_time = config.start_time;
    ctx.time_step = config.time_step;
    ctx.step_number = 0;

    for (size_t i = 0; i < models.size(); ++i) {
        ctx.parameters = config.models[i].parameters;
        ctx.random_seed = config.models[i].seed;
        models[i]->initialize(ctx);
    }

    uint64_t total_steps = static_cast<uint64_t>(
        (config.end_time - config.start_time).count() / config.time_step.count());

    result.status = "running";
    for (uint64_t step = 0; step < total_steps; ++step) {
        ctx.step_number = step;
        ctx.current_time = config.start_time +
            etwin::core::Nanoseconds(static_cast<int64_t>(step * config.time_step.count()));

        for (size_t i = 0; i < models.size(); ++i) {
            ctx.parameters = config.models[i].parameters;
            models[i]->step(ctx);
        }

        for (const auto& model : models) {
            auto outputs = model->output();
            for (const auto& [name, field] : outputs) {
                result.final_state.add_field(field);
            }
        }
    }

    for (auto& model : models) {
        model->finalize(ctx);
    }

    result.success = true;
    result.status = "completed";
    result.run_hash = result.final_state.compute_hash();
    return result;
}

ScenarioResult ScenarioEngine::run_baseline(const ScenarioConfig& config) {
    return run(config);
}

ScenarioResult ScenarioEngine::run_intervention(const ScenarioConfig& base,
                                                const Intervention& intervention) {
    ScenarioConfig modified = base;
    modified.scenario_id = base.scenario_id + "_" + intervention.name;
    modified.interventions.push_back(intervention);

    for (auto& mc : modified.models) {
        if (mc.model_name == intervention.target_model) {
            mc.parameters[intervention.parameter] = intervention.value;
        }
    }
    return run(modified);
}

std::map<std::string, ScenarioResult> ScenarioEngine::run_ensemble(
    const ScenarioConfig& config, size_t n_members, uint64_t base_seed) {
    std::map<std::string, ScenarioResult> results;
    for (size_t i = 0; i < n_members; ++i) {
        ScenarioConfig member = config;
        member.scenario_id = config.scenario_id + "_member_" + std::to_string(i);
        uint64_t seed = base_seed + i;
        for (auto& mc : member.models) mc.seed = seed;
        results[member.scenario_id] = run(member);
    }
    return results;
}

std::map<std::string, ScenarioResult> ScenarioEngine::run_sensitivity(
    const ScenarioConfig& config, const std::string& parameter,
    const std::vector<double>& values) {
    std::map<std::string, ScenarioResult> results;
    for (double v : values) {
        ScenarioConfig modified = config;
        modified.scenario_id = config.scenario_id + "_" + parameter + "_" + std::to_string(v);
        for (auto& mc : modified.models) {
            mc.parameters[parameter] = v;
        }
        results[modified.scenario_id] = run(modified);
    }
    return results;
}

} // namespace etwin::scenario
