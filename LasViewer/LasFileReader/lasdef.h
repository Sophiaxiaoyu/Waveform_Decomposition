#ifndef LAS_DEFINITIONS_H
#define LAS_DEFINITIONS_H
//
#pragma pack(push)
#pragma pack(1)  
typedef struct tagLAS_VLR_header
{
	unsigned short reserved;
	char user_id[16]; 
	unsigned short record_id;
	unsigned short record_length_after_header;
	char description[32];
}LAS_VLR_header;//Variable Length Record Header

//User_ID	"LASF_Spec"
//Record_ID		n			(100<=n<=356)
typedef struct tagLAS_Waveform_Packect_Descriptor
{
	unsigned char bits_per_sample;
	unsigned char waveform_compression_type;
	unsigned long number_of_samples;
	unsigned long temporal_sample_spacing;
	double		  digtizer_gain;
	double		  digtizer_offest;
}LAS_Waveform_Packect_Descriptor;//Waveform Packet Descriptor User Defined Record


typedef struct tagLAS_Public_header
{
	char file_signature[4];
	unsigned short file_source_id;
	unsigned short global_encoding;
	unsigned long project_ID_GUID_data_1;
	unsigned short project_ID_GUID_data_2;
	unsigned short project_ID_GUID_data_3;
	unsigned char project_ID_GUID_data_4[8];
	unsigned char version_major;
	unsigned char version_minor;
	char system_identifier[32];
	char generating_software[32];
	unsigned short file_creation_day;
	unsigned short file_creation_year;
	unsigned short header_size;
	unsigned long offset_to_point_data;
	unsigned long number_of_variable_length_records;
	unsigned char point_data_format;
	unsigned short point_data_record_length;
	unsigned long number_of_point_records;
	unsigned long number_of_points_by_return[5];
	double x_scale_factor;
	double y_scale_factor;
	double z_scale_factor;
	double x_offset;
	double y_offset;
	double z_offset;
	double max_x;
	double min_x;
	double max_y;
	double min_y;
	double max_z;
	double min_z;
	unsigned __int64 start_of_waveform_data_packect_record;
}LAS_Public_header;


typedef struct tagLAS_Point_Format0
{
	long X;
	long Y;
	long Z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;
}LAS_Point_Format0;

typedef struct tagLAS_Point_Format1
{
	long X;
	long Y;
	long Z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;
	double gps_time;
}LAS_Point_Format1;

typedef struct tagLAS_Point_Format2
{
	long X;
	long Y;
	long Z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;
	unsigned short red;
	unsigned short green;
	unsigned short blue;
}LAS_Point_Format2;

typedef struct tagLAS_Point_Format3
{
	long X;
	long Y;
	long Z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;
	double gps_time;
	unsigned short red;
	unsigned short green;
	unsigned short blue;
}LAS_Point_Format3;

typedef struct tagLAS_Point_Format4
{
	long X;
	long Y;
	long Z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;
	double gps_time;
	unsigned char wave_packet_descriptor_index;
	unsigned  __int64 byte_offset_to_waveform_data;
	unsigned long waveform_packet_size_in_bytes;
	float return_point_waveform_location;
	float xt;
	float yt;
	float zt;
}LAS_Point_Format4;

typedef struct tagLAS_Point_Format5
{
	long X;
	long Y;
	long Z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;
	double gps_time;
	unsigned short red;
	unsigned short green;
	unsigned short blue;
	unsigned char wave_packet_descriptor_index;
	unsigned __int64 byte_offset_to_waveform_data;
	unsigned long waveform_packet_size_in_bytes;
	float return_point_waveform_location;
	float xt;
	float yt;
	float zt;
}LAS_Point_Format5,LAS_PT;
#pragma pack(pop)
#endif