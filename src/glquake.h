/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2007-2008 Kristian Duske
Copyright (C) 2010-2014 QuakeSpasm developers
Copyright (C) 2016 Axel Gneiting

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef GLQUAKE_H
#define GLQUAKE_H

#include "atomics.h"
#include "tasks.h"

void GL_WaitForDeviceIdle(void);
bool GL_BeginRendering(bool use_tasks, task_handle_t *begin_rendering_task,
                       int *width, int *height);
bool GL_AcquireNextSwapChainImage(void);
task_handle_t GL_EndRendering(bool use_tasks, bool use_swapchain);
void GL_SynchronizeEndRenderingTask(void);
void GL_UpdateDescriptorSets(void);

extern int glwidth, glheight;

// r_local.h -- private refresh defs

#define ALIAS_BASE_SIZE_RATIO (1.0 / 11.0)
// normalizing factor so player model works out to about
//  1 pixel per triangle
#define MAX_LBM_HEIGHT 480

#define TILE_SIZE 128 // size of textures generated by R_GenTiledSurf

#define SKYSHIFT 7
#define SKYSIZE (1 << SKYSHIFT)
#define SKYMASK (SKYSIZE - 1)

#define BACKFACE_EPSILON 0.01

#define MAX_GLTEXTURES 4096
#define MAX_SANITY_LIGHTMAPS 256

#define NUM_COLOR_BUFFERS 2
#define INITIAL_STAGING_BUFFER_SIZE_KB 16384

#define FAN_INDEX_BUFFER_SIZE 126

#define LIGHTMAP_BYTES 4

#define WATER_FIXED_ORDER                                                      \
  1 // stable draw order for water, draws water surfs using texture chains even
    // in indirect mode

void R_TimeRefresh_f(void);
void R_ReadPointFile_f(void);
texture_t *R_TextureAnimation(texture_t *base, int frame);

typedef enum {
  pt_static,
  pt_grav,
  pt_slowgrav,
  pt_fire,
  pt_explode,
  pt_explode2,
  pt_blob,
  pt_blob2
} ptype_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct particle_s {
  // driver-usable fields
  vec3_t org;
  float color;
  // drivers never touch the following fields
  struct particle_s *next;
  vec3_t vel;
  float ramp;
  float die;
  ptype_t type;
} particle_t;

#define P_INVALID -1
#ifdef PSET_SCRIPT
void PScript_InitParticles(void);
void PScript_Shutdown(void);
void PScript_DrawParticles(cb_context_t *cbx);
void PScript_DrawParticles_ShowTris(cb_context_t *cbx);
struct trailstate_s;
int PScript_ParticleTrail(vec3_t startpos, vec3_t end, int type,
                          float timeinterval, int dlkey, vec3_t axis[3],
                          struct trailstate_s **tsk);
int PScript_RunParticleEffectState(vec3_t org, vec3_t dir, float count,
                                   int typenum, struct trailstate_s **tsk);
void PScript_RunParticleWeather(vec3_t minb, vec3_t maxb, vec3_t dir,
                                float count, int colour, const char *efname);
void PScript_EmitSkyEffectTris(qmodel_t *mod, msurface_t *fa, int ptype);
int PScript_FindParticleType(const char *fullname);
int PScript_RunParticleEffectTypeString(vec3_t org, vec3_t dir, float count,
                                        const char *name);
int PScript_EntParticleTrail(vec3_t oldorg, entity_t *ent, const char *name);
int PScript_RunParticleEffect(vec3_t org, vec3_t dir, int color, int count);
void PScript_DelinkTrailstate(struct trailstate_s **tsk);
void PScript_ClearParticles(bool load);
void PScript_UpdateModelEffects(qmodel_t *mod);
void PScript_ClearSurfaceParticles(qmodel_t *mod); // model is being unloaded.

extern int r_trace_line_cache_counter;
#define InvalidateTraceLineCache()                                             \
  do {                                                                         \
    ++r_trace_line_cache_counter;                                              \
  } while (0);
#else
#define PScript_RunParticleEffectState(o, d, c, t, s) true
#define PScript_RunParticleEffectTypeString(o, d, c, n)                        \
  true // just unconditionally returns an error
#define PScript_EntParticleTrail(o, e, n) true
#define PScript_ParticleTrail(o, e, t, d, a, s) true
#define PScript_EntParticleTrail(o, e, n) true
#define PScript_RunParticleEffect(o, d, p, c) true
#define PScript_RunParticleWeather(min, max, d, c, p, n)
#define PScript_ClearSurfaceParticles(m)
#define PScript_DelinkTrailstate(tsp)
#define InvalidateTraceLineCache()
#endif

typedef struct vulkan_pipeline_layout_s {
  VkPipelineLayout handle;
  VkPushConstantRange push_constant_range;
} vulkan_pipeline_layout_t;

typedef struct vulkan_pipeline_s {
  VkPipeline handle;
  vulkan_pipeline_layout_t layout;
} vulkan_pipeline_t;

typedef struct vulkan_desc_set_layout_s {
  VkDescriptorSetLayout handle;
  int num_combined_image_samplers;
  int num_ubos;
  int num_ubos_dynamic;
  int num_storage_buffers;
  int num_input_attachments;
  int num_storage_images;
  int num_sampled_images;
  int num_acceleration_structures;
} vulkan_desc_set_layout_t;

typedef enum {
  VULKAN_MEMORY_TYPE_DEVICE,
  VULKAN_MEMORY_TYPE_HOST,
  VULKAN_MEMORY_TYPE_NONE,
} vulkan_memory_type_t;

typedef struct vulkan_memory_s {
  VkDeviceMemory handle;
  size_t size;
  vulkan_memory_type_t type;
} vulkan_memory_t;

#define WORLD_PIPELINE_COUNT 16
#define MODEL_PIPELINE_COUNT 6
#define FTE_PARTICLE_PIPELINE_COUNT 16
#define MAX_BATCH_SIZE 65536
#define NUM_WORLD_CBX 6
#define NUM_ENTITIES_CBX 6

typedef enum {
  PCBX_BUILD_ACCELERATION_STRUCTURES,
  PCBX_UPDATE_LIGHTMAPS,
  PCBX_UPDATE_WARP,
  PCBX_RENDER_PASSES,
  PCBX_NUM,
} primary_cb_contexts_t;

typedef enum {
  // Main render pass:
  SCBX_WORLD,
  SCBX_ENTITIES,
  SCBX_SKY,
  SCBX_ALPHA_ENTITIES_ACROSS_WATER,
  SCBX_WATER,
  SCBX_ALPHA_ENTITIES,
  SCBX_PARTICLES,
  SCBX_VIEW_MODEL,
  // UI render Pass:
  SCBX_GUI,
  SCBX_POST_PROCESS,
  SCBX_NUM,
} secondary_cb_contexts_t;

static const int SECONDARY_CB_MULTIPLICITY[SCBX_NUM] = {
    NUM_WORLD_CBX,    // SCBX_WORLD,
    NUM_ENTITIES_CBX, // SCBX_ENTITIES,
    1,                // SCBX_SKY,
    1,                // SCBX_ALPHA_ENTITIES_ACROSS_WATER,
    1,                // SCBX_WATER,
    1,                // SCBX_ALPHA_ENTITIES,
    1,                // SCBX_PARTICLES,
    1,                // SCBX_VIEW_MODEL,
    1,                // SCBX_GUI,
    1,                // SCBX_POST_PROCESS,
};

typedef struct cb_context_s {
  VkCommandBuffer cb;
  canvastype current_canvas;
  VkRenderPass render_pass;
  int render_pass_index;
  int subpass;
  vulkan_pipeline_t current_pipeline;
  uint32_t vbo_indices[MAX_BATCH_SIZE];
  unsigned int num_vbo_indices;
} cb_context_t;

typedef struct {
  VkDevice device;
  bool device_idle;
  bool validation;
  bool debug_utils;
  VkQueue queue;
  cb_context_t primary_cb_contexts[PCBX_NUM];
  cb_context_t *secondary_cb_contexts[SCBX_NUM];
  VkClearValue color_clear_value;
  VkFormat swap_chain_format;
  bool want_full_screen_exclusive;
  bool swap_chain_full_screen_exclusive;
  bool swap_chain_full_screen_acquired;
  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;
  VkPhysicalDeviceMemoryProperties memory_properties;
  uint32_t gfx_queue_family_index;
  VkFormat color_format;
  VkFormat depth_format;
  VkSampleCountFlagBits sample_count;
  bool supersampling;
  bool non_solid_fill;
  bool multi_draw_indirect;
  bool screen_effects_sops;

  // Instance extensions
  bool get_surface_capabilities_2;
  bool get_physical_device_properties_2;
  bool vulkan_1_1_available;

  // Device extensions
  bool dedicated_allocation;
  bool full_screen_exclusive;
  bool ray_query;

  // Buffers
  VkImage color_buffers[NUM_COLOR_BUFFERS];

  // Index buffers
  VkBuffer fan_index_buffer;

  // Staging buffers
  int staging_buffer_size;

  // Render passes
  VkRenderPass main_render_pass[2]; // stencil clear, stencil dont_care
  VkRenderPass warp_render_pass;

  // Pipelines
  vulkan_pipeline_t basic_alphatest_pipeline[2];
  vulkan_pipeline_t basic_blend_pipeline[2];
  vulkan_pipeline_t basic_notex_blend_pipeline[2];
  vulkan_pipeline_layout_t basic_pipeline_layout;
  vulkan_pipeline_t world_pipelines[WORLD_PIPELINE_COUNT];
  vulkan_pipeline_layout_t world_pipeline_layout;
  vulkan_pipeline_t raster_tex_warp_pipeline;
  vulkan_pipeline_t particle_pipeline;
  vulkan_pipeline_t sprite_pipeline;
  vulkan_pipeline_layout_t
      sky_pipeline_layout[2]; // one texture (cubemap-like), two textures
                              // (animated layers)
  vulkan_pipeline_t sky_stencil_pipeline[2];
  vulkan_pipeline_t sky_color_pipeline[2];
  vulkan_pipeline_t sky_box_pipeline;
  vulkan_pipeline_t sky_cube_pipeline[2];
  vulkan_pipeline_t sky_layer_pipeline[2];
  vulkan_pipeline_t alias_pipelines[MODEL_PIPELINE_COUNT];
  vulkan_pipeline_t md5_pipelines[MODEL_PIPELINE_COUNT];
  vulkan_pipeline_t postprocess_pipeline;
  vulkan_pipeline_t screen_effects_pipeline;
  vulkan_pipeline_t screen_effects_scale_pipeline;
  vulkan_pipeline_t screen_effects_scale_sops_pipeline;
  vulkan_pipeline_t cs_tex_warp_pipeline;
  vulkan_pipeline_t showtris_pipeline;
  vulkan_pipeline_t showtris_indirect_pipeline;
  vulkan_pipeline_t showtris_depth_test_pipeline;
  vulkan_pipeline_t showtris_indirect_depth_test_pipeline;
  vulkan_pipeline_t showbboxes_pipeline;
  vulkan_pipeline_t update_lightmap_pipeline;
  vulkan_pipeline_t update_lightmap_rt_pipeline;
  vulkan_pipeline_t indirect_draw_pipeline;
  vulkan_pipeline_t indirect_clear_pipeline;
  vulkan_pipeline_t ray_debug_pipeline;
#ifdef PSET_SCRIPT
  vulkan_pipeline_t fte_particle_pipelines[FTE_PARTICLE_PIPELINE_COUNT];
#endif

  // Descriptors
  VkDescriptorPool descriptor_pool;
  vulkan_desc_set_layout_t ubo_set_layout;
  vulkan_desc_set_layout_t single_texture_set_layout;
  vulkan_desc_set_layout_t input_attachment_set_layout;
  VkDescriptorSet screen_effects_desc_set;
  vulkan_desc_set_layout_t screen_effects_set_layout;
  vulkan_desc_set_layout_t single_texture_cs_write_set_layout;
  vulkan_desc_set_layout_t lightmap_compute_set_layout;
  VkDescriptorSet indirect_compute_desc_set;
  vulkan_desc_set_layout_t indirect_compute_set_layout;
  vulkan_desc_set_layout_t lightmap_compute_rt_set_layout;
  VkDescriptorSet ray_debug_desc_set;
  vulkan_desc_set_layout_t ray_debug_set_layout;
  vulkan_desc_set_layout_t joints_buffer_set_layout;

  // Samplers
  VkSampler point_sampler;
  VkSampler linear_sampler;
  VkSampler point_aniso_sampler;
  VkSampler linear_aniso_sampler;
  VkSampler point_sampler_lod_bias;
  VkSampler linear_sampler_lod_bias;
  VkSampler point_aniso_sampler_lod_bias;
  VkSampler linear_aniso_sampler_lod_bias;

  // Matrices
  float projection_matrix[16];
  float view_matrix[16];
  float view_projection_matrix[16];

  // Dispatch table
  PFN_vkCmdBindPipeline vk_cmd_bind_pipeline;
  PFN_vkCmdPushConstants vk_cmd_push_constants;
  PFN_vkCmdBindDescriptorSets vk_cmd_bind_descriptor_sets;
  PFN_vkCmdBindIndexBuffer vk_cmd_bind_index_buffer;
  PFN_vkCmdBindVertexBuffers vk_cmd_bind_vertex_buffers;
  PFN_vkCmdDraw vk_cmd_draw;
  PFN_vkCmdDrawIndexed vk_cmd_draw_indexed;
  PFN_vkCmdDrawIndexedIndirect vk_cmd_draw_indexed_indirect;
  PFN_vkCmdPipelineBarrier vk_cmd_pipeline_barrier;
  PFN_vkCmdCopyBufferToImage vk_cmd_copy_buffer_to_image;
  PFN_vkGetBufferDeviceAddressKHR vk_get_buffer_device_address;

  PFN_vkGetAccelerationStructureBuildSizesKHR
      vk_get_acceleration_structure_build_sizes;
  PFN_vkCreateAccelerationStructureKHR vk_create_acceleration_structure;
  PFN_vkDestroyAccelerationStructureKHR vk_destroy_acceleration_structure;
  PFN_vkCmdBuildAccelerationStructuresKHR vk_cmd_build_acceleration_structures;
  VkPhysicalDeviceAccelerationStructurePropertiesKHR
      physical_device_acceleration_structure_properties;

#ifdef _DEBUG
  PFN_vkCmdBeginDebugUtilsLabelEXT vk_cmd_begin_debug_utils_label;
  PFN_vkCmdEndDebugUtilsLabelEXT vk_cmd_end_debug_utils_label;
#endif
} vulkanglobals_t;

extern vulkanglobals_t vulkan_globals;
extern bool indirect;

//====================================================

extern int r_visframecount; // ??? what difs?
extern int r_framecount;
extern mplane_t frustum[4];
extern bool render_warp;
extern bool in_update_screen;
extern bool use_simd;
extern int render_scale;

//
// view origin
//
extern vec3_t vup;
extern vec3_t vpn;
extern vec3_t vright;
extern vec3_t r_origin;

//
// screen size info
//
extern refdef_t r_refdef;
extern mleaf_t *r_viewleaf, *r_oldviewleaf;
extern int
    d_lightstylevalue[MAX_LIGHTSTYLES]; // 8.8 fraction of base light value

extern cvar_t r_drawentities;
extern cvar_t r_drawworld;
extern cvar_t r_drawviewmodel;
extern cvar_t r_speeds;
extern cvar_t r_pos;
extern cvar_t r_waterwarp;
extern cvar_t r_fullbright;
extern cvar_t r_lightmap;
extern cvar_t r_wateralpha;
extern cvar_t r_lavaalpha;
extern cvar_t r_telealpha;
extern cvar_t r_slimealpha;
extern cvar_t r_dynamic;
extern cvar_t r_novis;
extern cvar_t r_scale;

extern cvar_t gl_polyblend;
extern cvar_t gl_nocolors;

// johnfitz -- polygon offset
#define OFFSET_NONE 0
#define OFFSET_DECAL 1

// johnfitz -- rendering statistics
extern atomic_uint32_t rs_brushpolys, rs_aliaspolys, rs_skypolys, rs_particles,
    rs_fogpolys;
extern atomic_uint32_t rs_dynamiclightmaps, rs_brushpasses, rs_aliaspasses;

extern atomic_uint64_t total_device_vulkan_allocation_size;
extern atomic_uint64_t total_host_vulkan_allocation_size;

// johnfitz -- track developer statistics that vary every frame
extern cvar_t devstats;
typedef struct {
  int packetsize;
  int edicts;
  int visedicts;
  int efrags;
  int tempents;
  int beams;
  int dlights;
} devstats_t;
extern devstats_t dev_stats, dev_peakstats;

// ohnfitz -- reduce overflow warning spam
typedef struct {
  double packetsize;
  double efrags;
  double beams;
  double varstring;
} overflowtimes_t;
extern overflowtimes_t
    dev_overflows; // this stores the last time overflow messages were
                   // displayed, not the last time overflows occured
#define CONSOLE_RESPAM_TIME 3 // seconds between repeated warning messages

typedef struct {
  float position[3];
  float texcoord[2];
  byte color[4];
} basicvertex_t;

// johnfitz -- moved here from r_brush.c
extern int gl_lightmap_format;

// keep in sync with world.frag
#define LMBLOCK_WIDTH                                                          \
  1024 // FIXME: make dynamic. if we have a decent card there's no real reason
       // not to use 4k or 16k (assuming there's no lightstyles/dynamics that
       // need uploading...)
#define LMBLOCK_HEIGHT                                                         \
  1024 // Alternatively, use texture arrays, which would avoid the need to
       // switch textures as often.

#define LM_CULL_BLOCK_W 128
#define LM_CULL_BLOCK_H 256

typedef struct lm_compute_workgroup_bounds_s {
  float mins[3];
  float maxs[3];
} lm_compute_workgroup_bounds_t;
COMPILE_TIME_ASSERT(lm_compute_workgroup_bounds_t,
                    sizeof(lm_compute_workgroup_bounds_t) == 24);

typedef struct glRect_s {
  unsigned short l, t, w, h;
} glRect_t;
typedef struct glMaxUsed_s {
  unsigned short w, h;
} glMaxUsed_t;
struct lightmap_s {
  gltexture_t *texture;
  gltexture_t *surface_indices_texture;
  gltexture_t *lightstyle_textures[MAXLIGHTMAPS * 3 / 4];
  VkDescriptorSet descriptor_set;
  uint32_t modified[TASKS_MAX_WORKERS]; // when using GPU lightmap update,
                                        // bitmap of lightstyles that will be
                                        // drawn using this lightmap (16..64
                                        // OR-folded into bits 16..31)
  VkBuffer workgroup_bounds_buffer;
  glRect_t rectchange;
  glMaxUsed_t lightstyle_rectused[1 + MAXLIGHTMAPS * 3 /
                                          4]; // [0]: surface_indices; [1,2,3]:
                                              // lightstyle_textures[0,1,2]

  lm_compute_workgroup_bounds_t global_bounds[LMBLOCK_HEIGHT / LM_CULL_BLOCK_H]
                                             [LMBLOCK_WIDTH / LM_CULL_BLOCK_W];
  byte active_dlights[LMBLOCK_HEIGHT / LM_CULL_BLOCK_H]
                     [LMBLOCK_WIDTH / LM_CULL_BLOCK_W];
  byte num_used_lightstyles[LMBLOCK_HEIGHT / LM_CULL_BLOCK_H]
                           [LMBLOCK_WIDTH / LM_CULL_BLOCK_W];
  byte used_lightstyles[LMBLOCK_HEIGHT / LM_CULL_BLOCK_H]
                       [LMBLOCK_WIDTH / LM_CULL_BLOCK_W][MAX_LIGHTSTYLES];
  int cached_light[MAX_LIGHTSTYLES];
  int cached_framecount;

  // the lightmap texture data needs to be kept in
  // main memory so texsubimage can update properly
  byte *data;                //[4*LMBLOCK_WIDTH*LMBLOCK_HEIGHT];
  byte *lightstyle_data[4];  //[4*LMBLOCK_WIDTH*LMBLOCK_HEIGHT];
  uint32_t *surface_indices; //[LMBLOCK_WIDTH*LMBLOCK_HEIGHT];
  lm_compute_workgroup_bounds_t
      *workgroup_bounds; //[(LMBLOCK_WIDTH/8)*(LMBLOCK_HEIGHT/8)];
};
extern struct lightmap_s *lightmaps;
extern int lightmap_count; // allocated lightmaps

extern bool r_fullbright_cheatsafe, r_lightmap_cheatsafe,
    r_drawworld_cheatsafe; // johnfitz

extern float map_wateralpha, map_lavaalpha, map_telealpha,
    map_slimealpha; // ericw
extern float
    map_fallbackalpha; // spike -- because we might want r_wateralpha to apply
                       // to teleporters while water itself wasn't watervised

extern task_handle_t prev_end_rendering_task;

// johnfitz -- fog functions called from outside gl_fog.c
void Fog_Update(float density, float red, float green, float blue, float time);
void Fog_ParseServerMessage(void);
void Fog_GetColor(float *c);
float Fog_GetDensity(void);
void Fog_ResetFade(void);
const char *Fog_GetFogCommand(bool always);
void Fog_EnableGFog(cb_context_t *cbx);
void Fog_DisableGFog(cb_context_t *cbx);
void Fog_SetupFrame(cb_context_t *cbx);
void Fog_NewMap(void);
void Fog_Init(void);

void R_NewGame(void);

void R_AnimateLight(void);
void R_BuildTopLevelAccelerationStructure(void *unused);
void R_UpdateLightmapsAndIndirect(void *unused);
void R_MarkSurfaces(bool use_tasks, task_handle_t before_mark,
                    task_handle_t *store_efrags, task_handle_t *cull_surfaces,
                    task_handle_t *chain_surfaces);
bool R_CullBox(vec3_t emins, vec3_t emaxs);
void R_StoreEfrags(efrag_t **ppefrag);
bool R_CullModelForEntity(entity_t *e);
void R_RotateForEntity(float matrix[16], vec3_t origin, vec3_t angles,
                       unsigned char scale);
void R_MarkLights(dlight_t *light, int num, mnode_t *node);

void R_InitParticles(void);
void R_DrawParticles(cb_context_t *cbx);
void CL_RunParticles(void);
void R_ClearParticles(void);

void R_TranslatePlayerSkin(int playernum);
void R_TranslateNewPlayerSkin(int playernum); // johnfitz -- this handles cases
                                              // when the actual texture changes
void R_UpdateWarpTextures(void *unused);

void R_MarkDeps(int combined_deps, int worker_index);

bool R_IndirectBrush(entity_t *e);

void R_DrawWorld(cb_context_t *cbx, int index);
void R_DrawAliasModel(cb_context_t *cbx, entity_t *e, int *aliaspolys);
void R_DrawBrushModel(cb_context_t *cbx, entity_t *e, int chain,
                      int *brushpolys, bool sort, bool water_opaque_only,
                      bool water_transparent_only);
void R_DrawSpriteModel(cb_context_t *cbx, entity_t *e);
void R_DrawIndirectBrushes(cb_context_t *cbx, bool draw_water,
                           bool transparent_water, bool draw_sky, int index);
void R_DrawIndirectBrushes_ShowTris(cb_context_t *cbx);

void R_DrawTextureChains_Water(cb_context_t *cbx, qmodel_t *model,
                               entity_t *ent, texchain_t chain,
                               bool opaque_only, bool transparent_only);

void GL_BuildLightmaps(void);
void GL_SetupIndirectDraws(void);
void GL_SetupLightmapCompute(void);
void GL_UpdateLightmapDescriptorSets(void);
void GL_DeleteBModelVertexBuffer(void);
void GL_DeleteBModelAccelerationStructures(void);
void GL_BuildBModelVertexBuffer(void);
void GL_BuildBModelAccelerationStructures(void);
void GL_PrepareSIMDAndParallelData(void);
void GLMesh_UploadBuffers(qmodel_t *m, aliashdr_t *hdr, unsigned short *indexes,
                          byte *vertexes, aliasmesh_t *desc,
                          jointpose_t *joints);
void GLMesh_DeleteAllMeshBuffers(void);

int R_LightPoint(vec3_t p, float ofs, lightcache_t *cache, vec3_t *lightcolor);

void GL_SubdivideSurface(msurface_t *fa);
void R_BuildLightMap(msurface_t *surf, byte *dest, int stride);
void R_RenderDynamicLightmaps(msurface_t *fa);
void R_UploadLightmaps(void);

void R_DrawWorld_ShowTris(cb_context_t *cbx);
void R_DrawBrushModel_ShowTris(cb_context_t *cbx, entity_t *e);
void R_DrawAliasModel_ShowTris(cb_context_t *cbx, entity_t *e);
void R_DrawParticles_ShowTris(cb_context_t *cbx);
void R_DrawSpriteModel_ShowTris(cb_context_t *cbx, entity_t *e);

void DrawGLPoly(cb_context_t *cbx, glpoly_t *p, float color[3], float alpha);
void GLMesh_DeleteMeshBuffers(aliashdr_t *hdr);
void GL_MakeAliasModelDisplayLists(qmodel_t *m, aliashdr_t *hdr);

void Sky_Init(void);
void Sky_ClearAll(void);
void Sky_DrawSky(cb_context_t *cbx);
bool Sky_NeedStencil();
void Sky_NewMap(void);
void Sky_LoadTexture(qmodel_t *mod, texture_t *mt, int tex_index);
void Sky_LoadTextureQ64(qmodel_t *mod, texture_t *mt, int tex_index);
void Sky_LoadSkyBox(const char *name);
const char *Sky_GetSkyCommand(bool always);
void Sky_SetSkyfog(float value);

void R_ClearTextureChains(qmodel_t *mod, texchain_t chain);
void R_ChainSurface(msurface_t *surf, texchain_t chain);
void R_DrawTextureChains(cb_context_t *cbx, qmodel_t *model, entity_t *ent,
                         texchain_t chain);
void R_DrawWorld_Water(cb_context_t *cbx, bool transparent);

float GL_WaterAlphaForSurface(msurface_t *fa);

int GL_MemoryTypeFromProperties(uint32_t type_bits, VkFlags requirements_mask,
                                VkFlags preferred_mask);

void R_CreateDescriptorPool();
void R_CreateDescriptorSetLayouts();
void R_InitSamplers();
void R_CreatePipelineLayouts();
void R_CreatePipelines();
void R_DestroyPipelines();

#define MAX_PUSH_CONSTANT_SIZE                                                 \
  128 // Vulkan guaranteed minimum maxPushConstantsSize

static inline void R_BindPipeline(cb_context_t *cbx,
                                  VkPipelineBindPoint bind_point,
                                  vulkan_pipeline_t pipeline) {
  static byte zeroes[MAX_PUSH_CONSTANT_SIZE];
  assert(pipeline.handle != VK_NULL_HANDLE);
  assert(pipeline.layout.handle != VK_NULL_HANDLE);
  assert(cbx->current_pipeline.layout.push_constant_range.size <=
         MAX_PUSH_CONSTANT_SIZE);
  if (cbx->current_pipeline.handle != pipeline.handle) {
    vulkan_globals.vk_cmd_bind_pipeline(cbx->cb, bind_point, pipeline.handle);
    if ((pipeline.layout.push_constant_range.size > 0) &&
        ((cbx->current_pipeline.layout.push_constant_range.stageFlags !=
          pipeline.layout.push_constant_range.stageFlags) ||
         (cbx->current_pipeline.layout.push_constant_range.size !=
          pipeline.layout.push_constant_range.size)))
      vulkan_globals.vk_cmd_push_constants(
          cbx->cb, pipeline.layout.handle,
          pipeline.layout.push_constant_range.stageFlags, 0,
          pipeline.layout.push_constant_range.size, zeroes);
    cbx->current_pipeline = pipeline;
  }
}

static inline void R_PushConstants(cb_context_t *cbx,
                                   VkShaderStageFlags stage_flags, int offset,
                                   int size, const void *data) {
  vulkan_globals.vk_cmd_push_constants(cbx->cb,
                                       cbx->current_pipeline.layout.handle,
                                       stage_flags, offset, size, data);
}

static inline void R_BeginDebugUtilsLabel(cb_context_t *cbx, const char *name) {
#ifdef _DEBUG
  ZEROED_STRUCT(VkDebugUtilsLabelEXT, label);
  label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
  label.pLabelName = name;
  if (vulkan_globals.vk_cmd_begin_debug_utils_label)
    vulkan_globals.vk_cmd_begin_debug_utils_label(cbx->cb, &label);
#endif
}

static inline void R_EndDebugUtilsLabel(cb_context_t *cbx) {
#ifdef _DEBUG
  if (vulkan_globals.vk_cmd_end_debug_utils_label)
    vulkan_globals.vk_cmd_end_debug_utils_label(cbx->cb);
#endif
}

void R_AllocateVulkanMemory(vulkan_memory_t *memory,
                            VkMemoryAllocateInfo *memory_allocate_info,
                            vulkan_memory_type_t type,
                            atomic_uint32_t *num_allocations);
void R_FreeVulkanMemory(vulkan_memory_t *memory,
                        atomic_uint32_t *num_allocations);

void R_CreateBuffer(VkBuffer *buffer, vulkan_memory_t *memory,
                    const size_t size, VkBufferUsageFlags usage,
                    const VkFlags mem_requirements_mask,
                    const VkFlags mem_preferred_mask,
                    atomic_uint32_t *num_allocations,
                    VkDeviceAddress *device_address, const char *name);
void R_FreeBuffer(const VkBuffer buffer, vulkan_memory_t *memory,
                  atomic_uint32_t *num_allocations);

typedef struct buffer_create_info_s {
  VkBuffer *buffer;
  size_t size;
  size_t alignment;
  VkBufferUsageFlags usage;
  void **mapped;
  VkDeviceAddress *address;
  const char *name;
} buffer_create_info_t;
size_t
R_CreateBuffers(const int num_buffers, buffer_create_info_t *create_infos,
                vulkan_memory_t *memory, const VkFlags mem_requirements_mask,
                const VkFlags mem_preferred_mask,
                atomic_uint32_t *num_allocations, const char *memory_name);
void R_FreeBuffers(const int num_buffers, VkBuffer *buffers,
                   vulkan_memory_t *memory, atomic_uint32_t *num_allocations);

VkDescriptorSet R_AllocateDescriptorSet(vulkan_desc_set_layout_t *layout);
void R_FreeDescriptorSet(VkDescriptorSet desc_set,
                         vulkan_desc_set_layout_t *layout);

void R_InitStagingBuffers(void);
void R_SubmitStagingBuffers(void);
byte *R_StagingAllocate(int size, int alignment, VkCommandBuffer *cb_context,
                        VkBuffer *buffer, int *buffer_offset);
void R_StagingBeginCopy(void);
void R_StagingEndCopy(void);
void R_StagingUploadBuffer(const VkBuffer buffer, const size_t size,
                           const byte *data);

void R_InitGPUBuffers(void);
void R_InitMeshHeap(void);
glheapstats_t *R_GetMeshHeapStats(void);
void R_SwapDynamicBuffers(void);
void R_FlushDynamicBuffers(void);
void R_CollectDynamicBufferGarbage(void);
void R_CollectMeshBufferGarbage(void);
byte *R_VertexAllocate(int size, VkBuffer *buffer, VkDeviceSize *buffer_offset);
byte *R_IndexAllocate(int size, VkBuffer *buffer, VkDeviceSize *buffer_offset);
byte *R_UniformAllocate(int size, VkBuffer *buffer, uint32_t *buffer_offset,
                        VkDescriptorSet *descriptor_set);
byte *R_StorageAllocate(int size, VkBuffer *buffer, VkDeviceSize *buffer_offset,
                        VkDeviceAddress *device_address);

void R_AllocateLightmapComputeBuffers();

void GL_SetObjectName(uint64_t object, VkObjectType object_type,
                      const char *name);

#endif /* GLQUAKE_H */